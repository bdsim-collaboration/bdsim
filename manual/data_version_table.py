"""Generate the output class data version table for the release notes.

Each ROOT output class header in include/ is compared against its content in
the last git tag reachable from the current branch. The ClassDef() macro at the
bottom of each header gives the data version, so we get the old version from
the file as it was at the tag and the new version from the working tree.

Note the "Changed" column is a comparison of the code in the header with
comments, blank lines, whitespace and the ClassDef line itself removed - so a
new copyright year or doxygen comment doesn't count as a change. It flags the
classes to look at; whether the data members or their order actually changed
still has to be judged from the diff.

Usage:

  python data_version_table.py                # table for the terminal
  python data_version_table.py --format grid  # rst grid table for the manual
  python data_version_table.py --ref v1.7.7   # compare against another tag
"""

import argparse
import difflib
import glob
import os
import re
import subprocess

import tabulate

# ClassDef(BDSOutputROOTEventModel, 7);
CLASS_DEF_RX = re.compile(r"ClassDef\s*\(\s*(?P<name>\w+)\s*,\s*(?P<version>\d+)\s*\)")

# all candidate headers - those without a ClassDef are skipped
HEADER_GLOB = "include/BDSOutputROOT*.hh"

# classes whose data members come partly from a base class in another file
EXTRA_SOURCES = {"BDSOutputROOTEventBeam"    : ["parser/beamBase.h"],
                 "BDSOutputROOTEventOptions" : ["parser/optionsBase.h"]}

HEADERS = ["Class", "Changed", "Old Version", "New Version"]


def _git(*arguments, repository=None, check=True):
    """Run git in repository and return (return code, stdout)."""
    result = subprocess.run(["git"] + list(arguments),
                            cwd=repository,
                            capture_output=True,
                            text=True)
    if check and result.returncode != 0:
        command = " ".join(["git"] + list(arguments))
        raise RuntimeError(f"\"{command}\" failed: {result.stderr.strip()}")
    return result.returncode, result.stdout


def repository_root(path="."):
    """Top level directory of the git repository containing path."""
    return _git("rev-parse", "--show-toplevel", repository=path)[1].strip()


def last_tag(repository, ref="HEAD"):
    """Most recent tag reachable from ref."""
    return _git("describe", "--tags", "--abbrev=0", ref, repository=repository)[1].strip()


def class_def(contents, name=None):
    """(class name, data version) from the ClassDef in contents, else None.

    If name is given, only a ClassDef for that class is matched.
    """
    for match in CLASS_DEF_RX.finditer(contents or ""):
        if name is None or match.group("name") == name:
            return match.group("name"), int(match.group("version"))
    return None


def file_at_ref(repository, ref, path):
    """Contents of path (relative to the repository) at ref, or None if absent."""
    code, contents = _git("show", f"{ref}:{path}", repository=repository, check=False)
    return contents if code == 0 else None


def code_lines(contents):
    """Code lines of contents - comments, blank lines and the ClassDef removed.

    Comments are stripped so that a new copyright year or an edited doxygen
    comment isn't reported as a change, and the ClassDef line because
    incrementing the data version is not itself a change of the data.
    """
    code = re.sub(r"/\*.*?\*/", "", contents, flags=re.DOTALL)  # block comments
    code = re.sub(r"//[^\n]*", "", code)                        # line comments
    lines = []
    for line in code.splitlines():
        line = " ".join(line.split())  # normalise whitespace
        if not line or CLASS_DEF_RX.search(line):
            continue
        lines.append(line)
    return lines


def compare_file(repository, ref, path):
    """(number of added, number of removed) code lines of path since ref."""
    new_contents = ""
    absolute = os.path.join(repository, path)
    if os.path.exists(absolute):
        with open(absolute) as f:
            new_contents = f.read()
    new = code_lines(new_contents)

    old_contents = file_at_ref(repository, ref, path)
    if old_contents is None:
        return len(new), 0  # new file since ref
    old = code_lines(old_contents)

    added = removed = 0
    for line in difflib.unified_diff(old, new, n=0, lineterm=""):
        if line.startswith(("+++", "---", "@@")):
            continue
        if line.startswith("+"):
            added += 1
        elif line.startswith("-"):
            removed += 1
    return added, removed


def changed_files(repository, ref, paths):
    """[(path, added, removed)] for those paths whose code differs from ref."""
    result = []
    for path in paths:
        added, removed = compare_file(repository, ref, path)
        if added or removed:
            result.append((path, added, removed))
    return result


def collect(repository, ref):
    """Build the table data and a list of warnings for the given repository."""
    data = []
    warnings = []
    for path in sorted(glob.glob(os.path.join(repository, HEADER_GLOB))):
        relative = os.path.relpath(path, repository)
        with open(path) as f:
            current = class_def(f.read())
        if current is None:
            continue  # not a data class, e.g. a LinkDef or an interface header
        name, new_version = current

        sources = [relative] + EXTRA_SOURCES.get(name, [])
        differences = changed_files(repository, ref, sources)
        changed = bool(differences)
        summary = ", ".join(f"{p} (+{a}/-{r})" for p, a, r in differences)

        old_contents = file_at_ref(repository, ref, relative)
        if old_contents is None:
            old_version = None  # new file since ref - nothing to compare against
            warnings.append(f"{name}: {relative} is new since {ref}")
        else:
            previous = class_def(old_contents, name)
            old_version = previous[1] if previous else None
            if old_version is None:
                warnings.append(f"{name}: no ClassDef found in {relative} at {ref}")

        if changed and old_version is not None and new_version == old_version:
            warnings.append(f"{name}: changed since {ref} but data version is still "
                            f"{new_version} - check whether the data members or their "
                            f"order changed in {summary}")
        if not changed and old_version is not None and new_version != old_version:
            warnings.append(f"{name}: data version {old_version} -> {new_version} but no "
                            f"change detected since {ref}")

        data.append([name,
                     "Y" if changed else "N",
                     "-" if old_version is None else str(old_version),
                     str(new_version),
                     summary])
    return data, warnings


def generate_table(ref=None, repository=None, tablefmt="outline", verbose=False):
    """Print the data version table comparing the working tree against ref.

    ref         : git tag / commit to compare against (default the last tag).
    repository  : path inside the repository (default the current directory).
    tablefmt    : any tabulate format - "grid" gives an rst table.
    verbose     : also print which files differ for each changed class.
    """
    repository = repository_root(repository or ".")
    ref = ref or last_tag(repository)

    data, warnings = collect(repository, ref)
    rows = [record[:-1] for record in data]

    headers = HEADERS
    if tablefmt == "grid":  # rst grid table for the manual
        headers = [f"**{h}**" for h in HEADERS]

    print(f"Repository : {repository}")
    print(f"Comparing  : {ref} -> working tree\n")
    print(tabulate.tabulate(rows, headers, tablefmt=tablefmt, disable_numparse=True,
                            colalign=("left",) * len(HEADERS)))

    if verbose:
        print("\nChanged files (+added/-removed code lines):")
        for name, changed, _, _, summary in data:
            if changed == "Y":
                print(f"  {name}: {summary}")

    if warnings:
        print("\nWarnings:")
        for warning in warnings:
            print(f"  * {warning}")

    return data


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--ref", default=None,
                        help="tag or commit to compare against (default: last tag)")
    parser.add_argument("--repository", default=None,
                        help="path inside the bdsim repository (default: current directory)")
    parser.add_argument("--format", dest="tablefmt", default="grid",
                        help="tabulate table format, e.g. outline, grid (rst), github")
    parser.add_argument("--verbose", action="store_true",
                        help="list the files that differ for each changed class")
    arguments = parser.parse_args()
    generate_table(ref=arguments.ref,
                   repository=arguments.repository,
                   tablefmt=arguments.tablefmt,
                   verbose=arguments.verbose)


if __name__ == "__main__":
    main()
