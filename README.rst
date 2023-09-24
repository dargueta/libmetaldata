Data Structures for Low-Level Software
======================================

If you need linked lists, maps, and basic I/O that works on bare metal (*no*
operating system), then this library is for you.

I've found that for `MetalC <https://github.com/dargueta/metalc>`_ and a couple
other things I'm doing, I'm reusing a lot of the same code. I decided to extract
it into a separate library.

Building
--------

.. code-block:: sh

    # Set up the build system for your desired target platform
    ./configure [options]

    # Build everything
    make

    # Install the library
    make install

For a list of options, run `./configure -h`.

System Requirements
~~~~~~~~~~~~~~~~~~~

* Make (NMake will not work)
* C compiler implementing most of C99 and accepting at least a limited subset of
  GCC-like command-line arguments.

Supported Platforms
~~~~~~~~~~~~~~~~~~~

+-----------------+----------+-----------------------+-----------------------+
| Compiler        | OS       | Hosted                | Unhosted              |
|                 |          +----------+------------+----------+------------+
|                 |          | Compiles | Tests Pass | Compiles | Tests Pass |
+=================+==========+==========+============+==========+============+
| Clang_          | MacOS    | Y        | Y          | Y        | Y          |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Y          | Y        | Y          |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| GCC_            | MacOS    | Y        | Y          | Y        | Y          |
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
| SDCC_           | MacOS    | [#]_     |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        |            | Y        |            |
|                 +----------+----------+------------+----------+------------+
|                 | Windows  |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| TCC_ [#]_ [#]_  | MacOS    |          |            | Y        |            |
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

Common Needs
~~~~~~~~~~~~

Here is a quick reference for needs you may encounter:

Installing to a specific directory
**********************************

Everything is installed relative to ``/usr/share`` on *NIX systems, or ``C:\Windows\System32``
on Windows. You can change where the files are installed in one of two ways,
either when you're configuring or when running ``make install``:

* ``./configure -i path/to/directory``
* ``make install INSTALL_TOP=path/to/directory``

The directory will be created if it doesn't already exist. The structure of the
directory follows the typical Unix convention, namely:

* Headers go in ``INSTALL_TOP/include/metaldata``
* Library goes in ``INSTALL_TOP/lib``
* PKG-CONFIG file goes in ``INSTALL_TOP/lib/pkgconfig``

This is also followed on Windows for the sake of simplicity.

Developing
----------

(TODO)

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
       fails without `this fix <https://github.com/nemequ/munit/issues/98>`_. If
       you're just building the library, none of that is necessary.
.. [#] On macOS compiler has a bug in it that causes a crash.
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
