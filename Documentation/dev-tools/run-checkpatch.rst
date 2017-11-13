.. Copyright 2017 Knut Omang <knut.omang@oracle.com>

Makefile support for systematic checkpatch testing
==================================================

The scripts/checkpatch.pl script is able to detect a lot of syntactic and
semantic issues with the code, and is also constantly evolving and detecting
more. In an ideal world, all source files should adhere to whatever rules
imposed by checkpatch.pl with all bells and whistles enabled, in a way that
checkpatch can be run as a reflex by developers (and by bots) from the top level
Makefile for every changing source file. In the real world however there's a
number of challenges:

* Sometimes there are valid reasons for accepting violations of a checkpatch
  rule, even if that rule is a sensible one in the general case.
* Some subsystems have different restrictions and requirements for checkpatch.
  (Ideally, the number of subsystems with differing restrictions and
  requirements will diminish over time.)
* Similarly, the kernel contains a lot of code that predates checkpatch, or at
  least some of the newer rules, and we would like checkpatch to evolve without
  requiring the need to fix all issues detected with it in the same commit.
* On the other hand, we want to make sure that files that are checkpatch clean
  (to some well defined extent, such as passing checkpatch with checks only for
  certain important types of issues) keep being so.

This is the purpose of supplying the option ``--ignore-cfg checkpatch.cfg`` to
``scripts/checkpatch.pl``. It will then look for a file named ``checkpatch.cfg``
in the current directory or alternatively in the directory of the source
file. If that file exists, checkpatch parses a set of rules from it, and use
them to determine how to invoke checkpatch for a particular file. The kernel
Makefile system supports using this feature as an integrated part of compiling
the code.

The ignore configuration file
-----------------------------

The ignore configuration file can be used to set policies and "rein in"
checkpatch errors piece by piece for a particular subsystem or driver.
The the following syntax is supported::

	# comments
	line_len <n>
	except checkpatch_type [files ...]
	pervasive checkpatch_type1 [checkpatch_type2 ...]

The ``line_len`` directive defines the upper bound of characters per line
tolerated in this directory. The ``except`` directive takes a checkpatch type
such as for example ``MACRO_ARG_REUSE``, and a set of files that should not be
subject to this particular check type.  You can run ``scripts/checkpatch.pl
--list-types`` to see what types that are available. The ``pervasive`` directive
disables the listed types of checks for all the files in the directory.  The
``except`` and ``pervasive`` directives can be used cumulatively to add more
exceptions.

Running checkpatch from make
----------------------------

You can run checkpatch subject to rules defined in ``checkpatch.cfg`` in the
directory of the source file by using "make P=1" to run checkpatch on all files
that gets recompiled, or "make P=2" to run checkpatch on all source files.

A make variable ``PF`` allows passing additional parameters to
checkpatch.pl. You can for instance use::

	make P=2 PF="--fix-inplace"

to have checkpatch trying to fix issues it finds - *make sure you have a clean
git tree and carefully review the output afterwards!* Combine this with
selectively enabling of types of errors via changes to the local
``checkpatch.cfg``, and you can focus on fixing up errors subsystem or driver by
driver on a type by type basis.

By default checkpatch will skip all files in directories without a
checkpatch.cfg file when invoked with the --ignore-cfg parameter.  This is to
allow builds with P=2 to pass even for subsystems that has not yet done anything
to rein in checkpatch errors. At some point when all subsystems and drivers
either have fixed all checkpatch errors or added proper checkpatch.cfg files,
this can be changed.

To force checkpatch to run a full run in directories without a checkpatch.cfg
file as well, use::

	make P=2 PF="--req-ignore-cfg"

If you like to see all the warnings and errors produced by checkpatch, ignoring
any checkpatch.cfg files, you can use::

	make -k P=2 PF="--no-ignore-cfg"

or for a specific module directory::

	make -k P=2 M=drivers/infiniband/core PF="--no-ignore-cfg"

with the -k option to ``make`` to let it continue upon errors.

Ever tightening checkpatch rules
--------------------------------

Commit the changes to checkpatch.cfg together with the code changes that fixes a
particular type of issue, this will allow automatic checkpatch testing. This way
we can ensure that new errors of that particular type do not inadvertently sneak
in again! This can be done at any subsystem or module maintainer's discretion
and at the right time without having to do it all at the same time.

Before submitting your changes, verify that "make P=2" passes with no errors.
