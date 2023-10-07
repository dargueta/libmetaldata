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

    # Install the library (may require sudo)
    make install

For a list of options, run `./configure -h`.

System Requirements
~~~~~~~~~~~~~~~~~~~

* Make (NMake will not work)
* C compiler implementing most of C99 and accepting at least a limited subset of
  GCC-like command-line arguments.

Supported Platforms
~~~~~~~~~~~~~~~~~~~

Legend:

* Y: Full support
* NS: Not supported for technical reasons
* w: Requires compiling tests with non-fatal warnings (pass ``-t`` to ``configure``)
* m: Requires compiling with minimal flags (pass ``-m`` to ``configure``)

+-----------------+----------+-----------------------+-----------------------+
| Compiler        | OS       | Hosted                | Unhosted              |
|                 |          +----------+------------+----------+------------+
|                 |          | Compiles | Tests Pass | Compiles | Tests Pass |
+=================+==========+==========+============+==========+============+
| Amsterdam       | MacOS    |          |            |          |            |
| Compiler        +----------+----------+------------+----------+------------+
| Kit (ACK_)      | Linux    |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| Clang_          | MacOS    | Y        | Y          | Y        | Y          |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Y          | Y        | Y          |
+-----------------+----------+----------+------------+----------+------------+
| GCC_            | MacOS    | Y        | Y          | Y        | Y          |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Y          | Y        | Y          |
+-----------------+----------+----------+------------+----------+------------+
| Intel           | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| MinGW_          | Linux    | Y [#]_   | NS         | Y        | NS         |
+-----------------+----------+----------+------------+----------+------------+
| OpenWatcom      | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+
| PCC_            | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Yw         | Y        | Yw         |
+-----------------+----------+----------+------------+----------+------------+
| SDCC_           | See below for the list of architectures this compiles for|
+-----------------+----------+----------+------------+----------+------------+
| TCC_            | MacOS    |          |            |          |            |
|                 +----------+----------+------------+----------+------------+
|                 | Linux    | Y        | Yt         | Y        | Yt         |
+-----------------+----------+----------+------------+----------+------------+
| Visual Studio   | Cygwin   |          |            |          |            |
+-----------------+----------+----------+------------+----------+------------+

The following architectures are tested. SDCC is the only toolchain that can be
used for most of these, and actual tests can't run on those because there's no
way to test the hardware.

====================== ======== ============
Achitecture            Compiles Passes Tests
====================== ======== ============
AMD64 (x86-64)         Y        Y
DS390                  Y
DS400                  Y
ez80-z80               Y
GBZ80
HC08 (Motorola 68HC08) Y
macOS ARM64            Y        Y
MCS-51                 Y
MOS 6502               NS [#]_
PDK13                  Y
PDK14                  Y
PDK15                  Y
R2K                    Y
R2KA                   NS
R3KA                   Y
S08                    Y
SM83                   NS
STM8                   Y
TININative             NS [#]_
TLCS 90                Y
x86-32
z180                   Y
z80                    Y
z80n                   NS
====================== ======== ============

Common Needs
~~~~~~~~~~~~

Here is a quick reference for needs you may encounter:

Installing to a specific directory
**********************************

Everything is installed relative to ``/usr/share`` on *NIX systems, including
Cygwin. You can change where the files are installed in one of two ways, either
when you're configuring or when running ``make install``:

* ``./configure -i path/to/directory``
* ``make install PREFIX=path/to/directory``

The directory will be created if it doesn't already exist. The structure of the
directory follows the typical Unix convention, namely:

* Headers go in ``PREFIX/include/metaldata``
* Library goes in ``PREFIX/lib``
* PKG-CONFIG file goes in ``PREFIX/lib/pkgconfig``

This is also followed on Cygwin  for the sake of simplicity.

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

.. [#] I used a cross-compiler here because my Cygwin  partition kicked the bucket.
.. [#] Requires SDCC 4.3+
.. [#] Triggers a bug in SDCC and crashes the compiler.

.. _ACK: https://tack.sourceforge.net/
.. _Clang: https://clang.llvm.org/
.. _GCC: https://gcc.gnu.org/
.. _MinGW: https://sourceforge.net/projects/mingw/
.. _PCC: http://pcc.ludd.ltu.se/
.. _SDCC: https://sdcc.sourceforge.net/
.. _TCC: https://bellard.org/tcc/
