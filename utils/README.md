# Utilities

These are a collection of developer tools or scripts for maintenance.

## Lines of Code

To estimate the lines of useful code in the project, two methods were used. Firstly, the `cloc` package (available in brew) and secondly with a simple python script. As of v1.8.0:

| Directory     |     Raw |   Blank |   Comment |   Code (cloc) |   Code (script) |   Code (script, substantive) |
|---------------|---------|---------|-----------|---------------|-----------------|------------------------------|
| src           | 114,922 |  16,199 |    16,292 |        82,431 |          82,610 |                       61,209 |
| include       |  48,142 |   9,501 |    16,558 |        22,083 |          22,083 |                       15,690 |
| analysis      |  14,557 |   2,421 |     3,141 |         8,995 |           8,996 |                        6,436 |
| parser        |  11,414 |   1,435 |     1,798 |         8,181 |           8,181 |                        6,563 |
| modules       |   8,720 |   1,283 |     1,312 |         6,125 |           6,125 |                        4,879 |
| python        |   5,484 |     808 |       853 |         3,823 |           3,823 |                        3,305 |
| test          |   1,477 |     209 |       268 |         1,000 |           1,000 |                          743 |
| comparator    |   1,257 |     187 |       227 |           843 |             843 |                          588 |
| examples      |     983 |     162 |       250 |           571 |             571 |                          415 |
| convert       |     581 |      87 |        95 |           399 |             399 |                          285 |
| src-external  |     442 |      42 |       147 |           253 |             253 |                          189 |
| interpolator  |     115 |      18 |        18 |            79 |              79 |                           49 |
| bdsim.cc      |      63 |       6 |        25 |            32 |              32 |                           19 |
| configuration |      36 |       8 |        26 |             2 |               2 |                            2 |
| TOTAL         | 208,193 |  32,366 |    41,010 |       134,817 |         134,997 |                      100,372 |

## Update Licecne Year

This is used to update the licence text at the top of every source file. It should be run by the developer from the root directory of BDSIM. The LICENCE.txt file should be edited first. If it is a different length, then first copy it to OLDLICENCE.txt, then run this script. After that, it can be remoed. Remember to check the few exception files - BDSG4* to remove this licence text.

## Analysis tools

- `chunker.py` and `chunkermp.py`. Apply bdsimCombine to chunks of files from a glob. `mp` is the multiprocessing parallel version.
- `missingseeds.py`. Identify from a directory of 1234_raw.root which seeds are missing.

## Old Setup Scripts

Historically, we had the Python utilities such as pybdsim as git submodules and they were located in this directory. These have since been removed. The only vestigate remaining is the pip commands to install them in `pyutils.sh` and `pyg4ometry.sh`.