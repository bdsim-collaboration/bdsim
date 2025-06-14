Contributing
************

BDSIM is open source software and the group highly encourages people to get in contact,
contribute and add to the code. We are more than happy to provide guidance, get feedback
and collaborate together.

Users may either get in touch and get write access to the repository after discussion
with the developers or fork the repository and make "pull requests" through git.

Git Convention
==============

Principles
----------

* No one should commit to the master branch (the only exception is the release manager).
* The master branch is the default people will get when cloning the repository and should only
  be a working tagged version. Therefore, the only commits in master are tagged versions. This
  is so that anyone getting the software will have a sane tested version that definitely works.
* develop is our main development branch and features are made off of this branch.
* develop should always compile and run - i.e. feature branches should be tested as best as
  possible before merging back into develop. It is tolerable for develop to break unintentionally.
* We generally follow `<https://nvie.com/posts/a-successful-git-branching-model/>`_.
* A new feature should have at least 1 (passing) test to demonstrate and exploit it.
* Documentation should be added to the manual before merging the branch.

Naming
------

* Feature branches should be descriptive and preferably named in the format "some-new-feature".
