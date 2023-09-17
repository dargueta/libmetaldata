Data Structures for Low-Level Software
======================================

Data structures and abstractions for low-level software

Building
--------

You can use

System Requirements
~~~~~~~~~~~~~~~~~~~

* Make (NMake will not work)
* C compiler implementing C99 and accepting at least a limited subset of GCC-like
  command-line arguments.

Compilers I'm attempting to support:


+-----------------+----------+-----------------------+-----------------------+
| Compiler        | OS       | Hosted                | Unhosted              |
|                 |          +----------+------------+----------+------------+
|                 |          | Compiles | Tests Pass | Compiles | Tests Pass |
+=================+==========+==========+============+==========+============+
| Clang_          | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Y          | Y        | Y          |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| GCC_            | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Y          | Y        | Y          |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| Intel           | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| MinGW_          | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y [#]_   |            | Y        |            |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| OpenWatcom      | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| PCC_ [#]_       | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Y          | Y        | Y          |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| SDCC_           | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        |            | Y        |            |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| TCC_ [#]_ [#]_  | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Y [#]_     | Y        | Y          |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| Visual Studio   | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+


Developing
----------

License
-------

Apache 2.0 License. See ``LICENSE.txt`` for the legal details.

Acknowledgments
---------------
This uses `µunit <https://nemequ.github.io/munit>`_ for running tests.

Footnotes
---------

.. [#] I used a cross-compiler here because my Windows partition kicked the bucket.
.. [#] Tests don't compile unless warnings are non-fatal, and linking tests
       fails without `this fix <https://github.com/nemequ/munit/issues/98>`_ If
       you're just building the library, none of that is necessary.
.. [#] Tests don't compile without `this fix <https://github.com/nemequ/munit/issues/97>`_.
.. [#] 64-bit only. 32-bit is missing a header on my system.
.. [#] On Linux, if testing you must configure this with the ``-w`` ("non-fatal
       warnings") flag. If you're just building the library, it's not necessary.

.. _Clang: https://clang.llvm.org/
.. _GCC: https://gcc.gnu.org/
.. _MinGW: https://sourceforge.net/projects/mingw/
.. _PCC: http://pcc.ludd.ltu.se/
.. _SDCC: https://sdcc.sourceforge.net/
.. _TCC: https://bellard.org/tcc/
