fasted
======

Fast edit distance implemented in C using OpenMP as a Python module.

Installing and Running
----------------------

To build and install this module, run the following::

    $ git clone git://github.com/petrh/fasted.git
    $ cd fasted
    $ python setup.py install
    $ python setup.py test

Source is available at http://github.com/petrh/fasted.

Included Algorithms
-------------------

Edit distance:

* Levenshtein

Example Usage
-------------

    >>> import fasted
    >>> fasted.edit_distance([[1, 2, 3], [ 2, 3, 4])
    2

Authors
-------

* Petr Hosek <petrhosek@gmail.com>

License
-------

BSD license
