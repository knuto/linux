.. Copyright 2004 Linus Torvalds
.. Copyright 2004 Pavel Machek <pavel@ucw.cz>
.. Copyright 2006 Bob Copeland <me@bobcopeland.com>

Sparse
======

Sparse is a semantic checker for C programs; it can be used to find a
number of potential problems with kernel code.  See
https://lwn.net/Articles/689907/ for an overview of sparse; this document
contains some kernel-specific sparse information.


Using sparse for typechecking
-----------------------------

"__bitwise" is a type attribute, so you have to do something like this::

        typedef int __bitwise pm_request_t;

        enum pm_request {
                PM_SUSPEND = (__force pm_request_t) 1,
                PM_RESUME = (__force pm_request_t) 2
        };

which makes PM_SUSPEND and PM_RESUME "bitwise" integers (the "__force" is
there because sparse will complain about casting to/from a bitwise type,
but in this case we really _do_ want to force the conversion). And because
the enum values are all the same type, now "enum pm_request" will be that
type too.

And with gcc, all the "__bitwise"/"__force stuff" goes away, and it all
ends up looking just like integers to gcc.

Quite frankly, you don't need the enum there. The above all really just
boils down to one special "int __bitwise" type.

So the simpler way is to just do::

        typedef int __bitwise pm_request_t;

        #define PM_SUSPEND ((__force pm_request_t) 1)
        #define PM_RESUME ((__force pm_request_t) 2)

and you now have all the infrastructure needed for strict typechecking.

One small note: the constant integer "0" is special. You can use a
constant zero as a bitwise integer type without sparse ever complaining.
This is because "bitwise" (as the name implies) was designed for making
sure that bitwise types don't get mixed up (little-endian vs big-endian
vs cpu-endian vs whatever), and there the constant "0" really _is_
special.

Using sparse for lock checking
------------------------------

The following macros are undefined for gcc and defined during a sparse
run to use the "context" tracking feature of sparse, applied to
locking.  These annotations tell sparse when a lock is held, with
regard to the annotated function's entry and exit.

__must_hold - The specified lock is held on function entry and exit.

__acquires - The specified lock is held on function exit, but not entry.

__releases - The specified lock is held on function entry, but not exit.

If the function enters and exits without the lock held, acquiring and
releasing the lock inside the function in a balanced way, no
annotation is needed.  The tree annotations above are for cases where
sparse would otherwise report a context imbalance.

Getting sparse
--------------

You can get latest released versions from the Sparse homepage at
https://sparse.wiki.kernel.org/index.php/Main_Page

Alternatively, you can get snapshots of the latest development version
of sparse using git to clone::

        git://git.kernel.org/pub/scm/devel/sparse/sparse.git

DaveJ has hourly generated tarballs of the git tree available at::

        http://www.codemonkey.org.uk/projects/git-snapshots/sparse/


Once you have it, just do::

        make
        make install

as a regular user, and it will install sparse in your ~/bin directory.

Using sparse
------------

Do a kernel make with "make C=1" to run sparse on all the C files that get
recompiled, or use "make C=2" to run sparse on the files whether they need to
be recompiled or not.  The latter is a fast way to check the whole tree if you
have already built it.

The "make C={1,2}" form of kernel make indirectly calls sparse via "runchecks",
which dependent on configuration and command line options may dispatch calls to
other checkers in addition to sparse. Details on how this works is covered
in Documentation/dev-tools/runchecks.rst .

The optional make variable CF can be used to pass arguments to runchecks for dispatch
to sparse. If sparse is the only tool enabled, any option not recognized by
runchecks will be forwarded to sparse. If more than one tool is active, you must
add the parameters you want sparse to get as a comma separated list prefixed by
``--to-sparse:``. If you want sparse to be the only checker run, and you want
some nice colored output, you can specify this as::

	make C=2 CF="--run:sparse --color"

This will cause sparse to be called for all files which are supported by a valid
runchecks configuration (again see Documentation/dev-tools/runchecks.rst for
details). If you want to run sparse on all files and ignore any missing
configuration files(s), just add ``-n`` to the list of options passed to
runchecks. This will cause runchecks to call sparse with all errors enabled for
all files even if no valid configuration is found in the tree for the source files.

By default "runchecks" is set to enable all sparse errors, but you can
configure what checks to be applied by sparse on a per file or per subsystem
basis. With the above invocation, make will fail and stop on the first file
encountered with sparse errors or warnings in it. If you want to continue
anyway, you can use::

	make C=2 CF="--run:sparse --color -w"
