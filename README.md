SPACK 

export env=/mnt/ssd/aryawu/spack-trace/env
export SPACK_ROOT=/mnt/ssd/aryawu/spack
export PATH=$SPACK_ROOT/bin:$PATH

. /mnt/ssd/aryawu/spack/share/spack/setup-env.sh
(base) aryawu@boa:~/spack/var/spack/environments/default/gmake/src$ spack env activate
==> Activated default environment in /mnt/ssd/aryawu/spack/var/spack/environments/default

Using gmake as an example:

Step 1: Develop the package:
spack develop <pkg>

(base) aryawu@boa:~/spack-trace$ spack develop gmake
==> Cloning source code for gmake@=4.4.1
==> Using cached archive: /mnt/ssd/aryawu/spack/var/spack/cache/_source-cache/archive/dd/dd16fb1d67bfab79a72f5e8390735c49e3e8e70b4945a15ab1f81ddb78658fb3.tar.gz
==> Updating view at /mnt/ssd/aryawu/spack/var/spack/environments/default/.spack-env/view

(base) aryawu@boa:~/spack-trace$ spack location -p gmake
/mnt/ssd/aryawu/.spack/package_repos/fncqgg4/repos/spack_repo/builtin/packages/gmake


Step 2: Cd into the source code directory
(base) aryawu@boa:~/.spack/package_repos/fncqgg4/repos/spack_repo/builtin/packages/gmake$ spack stage gmake
==> Using cached archive: /mnt/ssd/aryawu/spack/var/spack/cache/_source-cache/archive/dd/dd16fb1d67bfab79a72f5e8390735c49e3e8e70b4945a15ab1f81ddb78658fb3.tar.gz
==> Staged gmake in /tmp/aryawu/spack-stage/spack-stage-gmake-4.4.1-2ngndgntlx3boqvtcl7kkhj4vtcjmqdu

cd spack-stage-gmake-4.4.1-2ngndgntlx3boqvtcl7kkhj4vtcjmqdu/spack-src/src
(base) aryawu@boa:/tmp/aryawu/spack-stage/spack-stage-gmake-4.4.1-2ngndgntlx3boqvtcl7kkhj4vtcjmqdu/spack-src/src$ ls
amiga.c       debug.h     gettext.h        load.c      remake.c        version.c
amiga.h       default.c   gmk-default.h    main.c      remote-cstms.c  vmsdir.h
ar.c          dep.h       gmk-default.scm  makeint.h   remote-stub.c   vms_exit.c
arscan.c      dir.c       gnumake.h        misc.c      rule.c          vms_export_symbol.c
….

Step 3: Edit source
cp output.c output.c.bak

cp ~/spack-trace/output.c output.c
grep 'BUG-INJECTED-HERE' output.c

Step4: Rebuild
spack install --dirty --keep-stage gmake

Step5: Test
make ~/spack-trace/Makefile.test_bug
Or
./opt/spack/linux-icelake/gmake-4.4.1-bf6onyzhtpmdaqxdwqr53qgwi2gqhael/bin/make

(base) aryawu@boa:~/spack$ ./opt/spack/linux-icelake/gmake-4.4.1-bf6onyzhtpmdaqxdwqr53qgwi2gqhael/bin/make
=================================
  GNU Make Test Suite Running
=================================

Test 1: Accessing GNU Make globals
  ✓ version: 4.4.1

Test 2: Calling GNU Make function
  ✓ initialize_global_hash_tables() completed

Test 3: Checking flags
  silent_flag: 0
  debug_flag: 0

=================================
  All tests passed!
=================================


In general how to run it?
find $(spack location -i <pkg>) -type f -executable | grep -v ".so"
find $(spack location -i /2ngndgn) -type f -executable | grep -v ".so"
/mnt/ssd/aryawu/spack/opt/spack/linux-icelake/gmake-4.4.1-2ngndgntlx3boqvtcl7kkhj4vtcjmqdu/bin/make





——————————————————————————
So far above is only a way to do normal tests.
What about Unity testing framework?
We need a way to compile Unity before linking.
For coreutils we modified the Makefile.
But Spack does not have a global makefile. Build is often multi-step, multi-directory, with Autotools, CMake, or custom scripts

In Coreutils, including tests.c inside the program source lets you leverage the existing build system and Makefile: all compilation flags, object files, libraries, and linking steps are already handled.
You don’t need to figure out how to build a separate test executable from scratch — the program binary itself becomes the test runner.
For unity to work in coreutils I did LDADD = src/libver.a lib/libcoreutils.a unity/unity.o $(LIBINTL) $(MBRTOWC_LIB) \
        $(INTL_MACOSX_LIBS) lib/libcoreutils.a


But for spack, doing exactly what I did in Coreutils might requires patching the package’s build system (or spack install script) to:
    1. Compile unity.o
    2. Replace or inject the program’s main() with the test main
    3. Ensure all object files get linked correctly

Claude says: 
Option1: Modify package recipes to use unity.o
For each package you want to test, modify its package.py:

python
def setup_build_environment(self, env):
    # Add unity header to include path
    env.append_flags('CFLAGS', '-I/path/to/spack-unity')
    env.append_flags('CXXFLAGS', '-I/path/to/spack-unity')
    
    # Add unity.o to linker flags
    env.append_flags('LDFLAGS', '/path/to/spack-unity/unity.o')

Option2: Create a Unity Spack package
The more "Spack-native" approach:

create a unity package
class Unity(Package):
    """Unity Test Framework"""
    
    def install(self, spec, prefix):
        mkdirp(prefix.lib)
        mkdirp(prefix.include)
        
        gcc = which('gcc')
        gcc('-c', '-fPIC', '-o', 'unity.o', 'unity.c')
        
        install('unity.o', prefix.lib)
        install('unity.h', prefix.include)
Then packages can depends_on('unity', type='build') and link against it.

