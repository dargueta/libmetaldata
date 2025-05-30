Data Structures for Low-Level Software
======================================

If you need linked lists, maps, and basic I/O abstractions that work on bare
metal (i.e. *no* operating system), then this library is for you.

I've found that for `MetalC <https://github.com/dargueta/metalc>`_ and a couple
other things I'm doing, I'm reusing a lot of the same code. I decided to extract
it into a separate library.

Standard Library Usage
----------------------

The following, and *only* the following parts of the C standard library are used.
MetalData comes with some built-in fallbacks when compiled in unhosted
("freestanding" or "bare metal") mode. Unsupported functionality must be provided
by client code; more on that in the docs.

=========== ========= ===================
What        Header    Unhosted Fallbacks
=========== ========= ===================
bool        stdbool.h \*
free        stdlib.h  N
malloc      stdlib.h  N
memcmp      string.h  Y
memcpy      string.h  Y
memset      string.h  Y
realloc     stdlib.h  N
SIZE_MAX    stdint.h  \*
size_t      stddef.h  \*
strcmp      string.h  Y
uintmax_t   stdint.h  \*
UINTMAX_MAX stdint.h  \*
uintptr_t   stdint.h  (Only if available)
UINTPTR_MAX stdint.h  (Only if available)
=========== ========= ===================

*\*The C standard requires these to be available in unhosted mode. If you're
using a standards-compliant compiler, this won't be an issue.*

Building
--------

.. code-block:: shell

    # Set up the build system for your desired target platform
    ./configure [options]

    # Build everything
    make

    # Install the library (may require sudo)
    make install

For a list of options, run ``./configure -h``.

System Requirements
~~~~~~~~~~~~~~~~~~~

* Make (NMake will not work)
* C compiler implementing most of C99 and accepting at least a limited subset of
  GCC-like command-line arguments.

Supported Platforms
~~~~~~~~~~~~~~~~~~~

Legend:

* Y: Full support
* (blank): Untested or not implemented yet
* NS: Not supported for technical reasons
* m: Requires compiling with minimal flags (pass ``-F`` to ``configure``)

Compilers
*********

+-----------------+----------+-----------------------+------------------------+
| Compiler        | OS       | Hosted                | Unhosted               |
|                 |          +----------+------------+----------+-------------+
|                 |          | Compiles | Tests Pass | Compiles | Tests Pass  |
+=================+==========+==========+============+==========+=============+
| Clang_          | Cygwin   | Y        | Y          | Y        | Y           |
|                 +----------+----------+------------+----------+-------------+
|                 | Linux    | Y        | Y          | Y        | Y           |
|                 +----------+----------+------------+----------+-------------+
|                 | MacOS    | Y        | Y          | Y        | Y           |
+-----------------+----------+----------+------------+----------+-------------+
| GCC_            | Cygwin   | Y        | Y          | Y        | Y           |
|                 +----------+----------+------------+----------+-------------+
|                 | Linux    | Y        | Y          | Y        | Y           |
|                 +----------+----------+------------+----------+-------------+
|                 | MacOS    | Y        | Y          | Y        | Y           |
+-----------------+----------+----------+------------+----------+-------------+
| Intel           | MacOS    |          |            |          |             |
|                 +----------+----------+------------+----------+-------------+
|                 | Linux    |          |            |          |             |
+-----------------+----------+----------+------------+----------+-------------+
| MinGW_          | Cygwin   | Y        | Y          | Y        | Y           |
|                 +----------+----------+------------+----------+-------------+
|                 | Linux    | Y [#]_   | NS         | Y        | NS          |
+-----------------+----------+----------+------------+----------+-------------+
| OpenWatcom      | MacOS    |          |            |          |             |
|                 +----------+----------+------------+----------+-------------+
|                 | Linux    |          |            |          |             |
+-----------------+----------+----------+------------+----------+-------------+
| SDCC_           | See below for the list of architectures this compiles for |
+-----------------+----------+----------+------------+----------+-------------+
| TCC_            | Linux    | Y        | Y          | Y        | Y           |
+-----------------+----------+----------+------------+----------+-------------+
| Visual Studio   | Windows  |          |            |          |             |
+-----------------+----------+----------+------------+----------+-------------+

Architectures
*************

The following architectures are tested. SDCC is the only toolchain that can be
used for most of these. The microcontrollers require me to write a testbench
using uCSim_ which is going to be time-consuming but (possibly) doable.

====================== ======== ============
Architecture           Compiles Passes Tests
====================== ======== ============
AMD64 (x86-64)         Y        Y
DS390                  Y
DS400                  Y
ez80-z80               Y
HC08 (Motorola 68HC08) Y
macOS ARM64            Y        Y
MCS-51                 Y
MOS 6502               Y
MOS 65C02              Y
Padauk PDK 13          Y
Padauk PDK 14          Y
Padauk PDK 15          Y
Rabbit 800             Y
Rabbit 2000            Y
Rabbit 2000A           NS
Rabbit 3000A           Y
S08                    Y
SM83                   NS
STM8                   Y
TININative             Y [#]_
TLCS 90                Y
x86-32                 Y        Y
z180                   Y
z80                    Y
z80n                   NS
====================== ======== ============

Common Needs
~~~~~~~~~~~~

Here is a quick reference for needs you may encounter:

Installing to a specific directory
**********************************

By default, everything is installed relative to ``/usr/share`` on \*NIX systems,
including Cygwin. You can change where the files are installed during
configuration:

.. code-block:: shell

    ./configure --prefix=path/to/directory

The directory will be created if it doesn't already exist. The structure of the
directory follows the typical Unix convention, namely:

* Headers go in ``PREFIX/include/metaldata``
* Library goes in ``PREFIX/lib``
* PKG-CONFIG file goes in ``PREFIX/lib/pkgconfig``

This pattern is also followed on Cygwin for the sake of simplicity.

Developing
----------

Optional tools:

* valgrind
* kcallgrind (Debian package)
* massif-visualizer (Debian package)

License
-------

Mozilla 2.0 License. See ``LICENSE`` for the legal details.

Acknowledgments
---------------
This uses `µunit <https://nemequ.github.io/munit>`_ for running tests.

Footnotes
---------

.. [#] I used a cross-compiler here because my Windows partition kicked the bucket.
.. [#] Triggers a bug in SDCC and crashes the compiler.

.. _Clang: https://clang.llvm.org/
.. _GCC: https://gcc.gnu.org/
.. _MinGW: https://sourceforge.net/projects/mingw/
.. _SDCC: https://sdcc.sourceforge.net/
.. _TCC: https://bellard.org/tcc/
.. _uCSim: https://sdcc.sourceforge.net/
