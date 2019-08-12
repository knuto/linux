4. Building and installing KTF
------------------------------

KTF's user land side depends on googletest.
The googletest project has seen some structural changes in moving from a
project specific gtest-config via no package management support at all to
recently introduce pkgconfig support. This version of KTF only supports
building against a googletest (gtest) with pkgconfig support, which means
that as of February 2018 you have to build googletest from source at
github.

Googletest has also recently been fairly in flux, and while we
try to keep up to date with the official googletest version on Github,
we have seen issues with changes that breaks KTF. We also have a small
queue of enhancements and fixes to Googletest based on our experience
and use of it a.o. with KTF. You can find the latest rebase of this
version in the ktf branch of knuto/googletest at Github, but expect it
to rebase as we move forward to keep it up-to-date.
This version will at any time have been tested with KTF by us, since
we use it internally. Let's assume for the rest of these instructions
that your source trees are below ``~/src`` and your build trees are
under ``~/build``::

	cd ~/src
	git clone https://github.com/knuto/googletest.git

or::

        cd ~/src
        git clone https://github.com/google/googletest.git

then::

	mkdir -p ~/build/$(uname -r)
	cd ~/build/$(uname -r)
	mkdir googletest
	cd googletest
	cmake ~/src/googletest
	make
	sudo make install

Default for googletest is to use static libraries.  If you want to use shared
libraries for googletest, you can specify ``-DBUILD_SHARED_LIBS=ON`` to
cmake. If you don't want to install googletest into /usr/local, you can
specify an alternate install path using ``-DCMAKE_INSTALL_PREFIX=<your path>``
to cmake for googletest, and similarly use ``--prefix=<your path>`` both for
KTF and your own test modules. Note that on some distros, cmake version
2 and 3 comes as different packages, make sure you get version 3, which may
require you to use ``cmake3`` as command instead of cmake above.

Building the in-kernel version of KTF and running KTF selftests
***************************************************************

The environment needs to have the path to the
gtest (Googletest) build set to the directory above the lib and
include directories::

    export GTEST_PATH=$HOME/install

KTF can then be built using the module target, eg. from the top level
kernel build tree.

    make M=tools/testing/selftests/ktf

You can run also build (and run) KTF tests as selftests tests
via the kselftest target::

    make TARGETS="ktf" kselftest

You can invoke this command to let the tests run as a normal user, but
root access is needed to load and unload ktf.ko and the test
module(s). This will happen as part of the kselftest target even as a
normal user if the user has sudo privileges.
