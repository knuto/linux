.. Copyright 2017 Knut Omang <knut.omang@oracle.com>

Makefile support for running checkers
=====================================

Tools like sparse, coccinelle, and scripts/checkpatch.pl are able to detect a
lot of syntactic and semantic issues with the code, and are also constantly
evolving and detecting more. In an ideal world, all source files should
adhere to whatever rules imposed by checkpatch.pl and sparse etc. with all
bells and whistles enabled, in a way that these checkers can be run as a reflex
by developers (and by bots) from the top level Makefile for every changing
source file. In the real world however there's a number of challenges:

* Sometimes there are valid reasons for accepting violations of a checker
  rule, even if that rule is a sensible one in the general case.
* Some subsystems have different restrictions and requirements.
  (Ideally, the number of subsystems with differing restrictions and
  requirements will diminish over time.)
* Similarly, the kernel contains a lot of code that predates the tools, or at
  least some of the newer rules, and we would like these tools to evolve without
  requiring the need to fix all issues detected with it in the same commit.
  We also want to accommodate new tools, so that each new tool does not
  have to reinvent its own mechanism for running checks.
* On the other hand, we want to make sure that files that are clean
  (to some well defined extent, such as passing checkpatch or sparse
  with checks only for certain important types of issues) keep being so.

This is the purpose of ``scripts/runchecks``.

The ``runchecks`` program looks for files named ``runchecks.cfg`` in the
``scripts`` directory, then in the directory hierarchy of the source file,
starting from where the source file is located, searching upwards. If at least
one such file exists in the source tree, ``runchecks`` parses a set of
rules from it, and uses them to determine how to invoke a set of individual
checker tools for a particular file. The kernel Makefile system supports
this feature as an integrated part of compiling the code, using the
``C={1,2}`` option. With::

	make C=1

runchecks will be invoked if the file needs to be recompiled. With ::

	make C=2

runchecks will be invoked for all source files, even if they do not need
recompiling. Based on the configuration, ``runchecks`` will invoke one or
more checkers. The number and types of checkers to run are configurable and
can also be selected on the command line::

	make C=2 CF="--run:sparse,checkpatch"

If only one checker is run, any parameter that is not recognized by
``runchecks`` itself will be forwarded to the checker. If more than one checker
is enabled, parameters can be forwarded to a specific checker by means of
this syntax::

	make C=2 CF="--to-checkpatch:--terse"

A comma separated list of parameters can be supplied if necessary.

Supported syntax of the runchecks.cfg configuration file
--------------------------------------------------------

The ``runchecks`` configuration file chain can be used to set policies and "rein in"
checker errors piece by piece for a particular subsystem or driver. It can
also be used to mitigate and extend checkers that do not support
selective suppression of all it's checks.

Two classes of configuration are available. The first class is configuration
that defines what checkers are enabled, and some logic to allow better
suppression or a more unified output of warning messages.
This type of configuration should go into the first accessed
configuration file, and has been preconfigured for the currently supported
checkers in ``scripts/runchecks.cfg``. The second class is the features for
configuring the output of the checkers by selectively suppressing checks on
a per file or per check basis. These typically go in the source tree in
the directory of the source file or above. Some of the syntax is generic
and some is only supported by some checkers.

For the first class of configuration the following syntax is supported::

	# comments
	checker checkpatch [command]
	addflags <list of extra flags and parameters>
	cflags
	typedef NAME <regular expression>
	run [checker list|all]

The ``checker`` command switches ``runchecks``'s attention to a particular
checker. The following commands until the next ``checker`` statement
apply to that particular checker. The first occurrence of ``checker``
also serves as a potentially defining operation, if the checker name
has not been preconfigured. In that case, a second parameter can be used
to provide the name of the command used to run the checker.
A full checker integration into runchecks will typically require some
additions to runchecks, and will then have been preconfigured,
but simple checkers might just be configured on the fly.

The ``addflags`` command incrementally adds more flags and parameters to
the command line used to invoke the checker. This applies to all
invocations of the checker from runchecks.

The ``cflags`` command forwards all the flags and options passed to
the compiler invocation to the checker. The default is to suppress these
parameters when invoking the checker.

The ``typedef`` command adds ``NAME`` and associates it with the given regular
expression. This expression is used to match against standard error output from
the checker. In the kernel tree, ``NAME`` can then be used in local
``runcheck.cfg`` files as a new named check that runchecks understands and that
can be used with checker supported names below to selectively suppress that
particular set of warning or error messages. This is useful to handle output
checks for which the underlying checker does not provide any suppression.  Check
type namespaces are separate for the individual checkers. You can list the state
of the built in and configured checker and check types with::

	scripts/runchecks --list

The checker implementations of the ``typedef`` command also allow runchecks to
perform some unification of output by rewriting the output lines, and use of the
new type names in the error output, to ease the process of updating the
runchecks.cfg files.  It also adds some limited optional color support.  Having
a unified representation of the error output also makes it much easier to do
statistics or other operations on top of an aggregated output from several
checkers.

For the second class of configuration the following syntax is supported::

	# comments
	checker checker_name
	except check_type [files ...]
	pervasive check_type1 [check_type2 ...]
	line_len <n>

The ``except`` directive takes a check type such as for example
``MACRO_ARG_REUSE``, and a set of files that should not be subject to this
particular check type. The ``pervasive`` command disables the listed types
of checks for all the files in the subtree.  The ``except`` and
``pervasive`` directives can be used cumulatively to add more exceptions.
The ``line_len`` directive defines the upper bound of characters per line
tolerated in this directory. Currently only ``checkpatch`` supports this
command.

Options when running checker programs from make
-----------------------------------------------

A make variable ``CF`` allows passing additional parameters to
``runchecks``. You can for instance use::

	make C=2 CF="--run:checkpatch --fix-inplace"

to run only the ``checkpatch`` checker, and to have checkpatch try to fix
issues it finds - *make sure you have a clean git tree and carefully review
the output afterwards!* Combine this with selectively enabling of types of
errors via changes under ``checker checkpatch`` to the local
``runchecks.cfg``, and you can focus on fixing up errors subsystem or
driver by driver on a type by type basis.

By default runchecks will skip all files if a ``runchecks.cfg`` file cannot
be found in the directory of the file or in the tree above.  This is to
allow builds with ``C=2`` to pass even for subsystems that have not yet done
anything to rein in checker errors. At some point when all subsystems and
drivers either have fixed all checker errors or added proper
``runchecks.cfg`` files, this can be changed.
Note that the runchecks.cfg file in the scripts/ directory is special, in that
it can be present without triggering checker runs in the main kernel tree.

To force runchecks to run a full run in directories/trees where runchecks
does not find  a ``runchecks.cfg`` file as well, use::

	make C=2 CF="-f"

If you like to see all the warnings and errors produced by the checkers, ignoring
any runchecks.cfg files except the one under ``scripts``, you can use::

	make C=2 CF="-n"

or for a specific module directory::

	make C=2 M=drivers/infiniband/core CF="--color -n -w"

with the -w option to ``runchecks`` to suppress errors from any of the
checkers and just continue on, and the ``--color`` option to present errors
with colors where supported.

Ever tightening checker rules
-----------------------------

Commit the changes to the relevant ``runchecks.cfg`` together with the code
changes that fixes a particular type of issue, this will allow automatic
checker running by default. This way we can ensure that new errors of that
particular type do not inadvertently sneak in again! This can be done at
any subsystem or module maintainer's discretion and at the right time
without having to do it all at the same time.

Before submitting your changes, verify that a full "make C=2" passes
with no errors.

Extending and improving checker support in ``runchecks``
--------------------------------------------------------

The runchecks program has been written with extensibility in mind.
If the checker starts its reporting lines with filename:lineno, there's a
good chance that a new checker can simply be added by adding::

	checker mychecker path_to_mychecker

to ``scripts/runchecks.cfg`` and suitable ``typedef`` expressions to provide
selective suppressions of output, however it is likely that some quirks are
needed to make the new checker behave similarly to the others, and to support
the full set of features, such as the ``--list`` option. This is done by
implementing a new subclass of the Checker class in ``runchecks``. This is the
way all the available default supported checkers are implemented, and those
relatively lean implementations could serve as examples for support for future
checkers.
