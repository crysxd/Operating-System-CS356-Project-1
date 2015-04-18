GENRAL:
	- Sample runs are provided in "Problem X/log"
	- All binaries are build in "bin/"
	- StopWatch (problem 4) and ForkCopy (problem 2) must be runned in "bin/" directory!
	- Documentation for each program is provied in Documentation.pdf

SUBMITTED FILES:
	- makefile					| makefile building all binaries to bin/
	- Prj1README.txt			| readme file
	- Documentation.pdf			| small documentation for all problems
	- log.txt					| log file containing runs of all programs
	- Problem 1					| 
		- MyCopy.c 				| implementation of problem 1
	- Problem 2					| 
		- ForkCopy.c 			| implementation of problem 2
	- Problem 3					| 
		- PipeCopy.c 			| implementation of problem 3
	- Problem 4					| 
		- StopWatch.c 			| programm calling all copy programs to test their performance
	- Problem 5					| 
		- MyShell.h 			| implementation of problem 5 (header file)
		- MyShell.c 			| implementation of problem 5
	- Problem 6					| 
		- MoreShell.h 			| implementation of problem 6 (header file)
		- MoreShell.c 			| implementation of problem 6
	- Problem 7					| 
		- DupShell.h 			| implementation of problem 7 (header file)
		- DupShell.c 			| implementation of problem 7
	- Problem 8					| 
		- MergesortSignle.c		| implementation of problem 8 (single threaded)
		- MergesortMulti.c		| implementation of problem 8 (multi threaded)
	- Problem 9					| 
		- BurgerBuddies.c		| implementation of problem 9

TEST ENVIRONMENT:
	- Ubuntu 13.04
	- GCC version information (gcc -v):
		Using built-in specs.
		COLLECT_GCC=gcc
		COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/4.7/lto-wrapper
		Target: x86_64-linux-gnu
		Configured with: ../src/configure -v --with-pkgversion='Ubuntu/Linaro 4.7.3-1ubuntu1' --with-bugurl=file:///usr/share/doc/gcc-4.7/README.Bugs --enable-languages=c,c++,go,fortran,objc,obj-c++ --prefix=/usr --program-suffix=-4.7 --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --with-gxx-include-dir=/usr/include/c++/4.7 --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --enable-gnu-unique-object --enable-plugin --with-system-zlib --enable-objc-gc --with-cloog --enable-cloog-backend=ppl --disable-cloog-version-check --disable-ppl-version-check --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --with-tune=generic --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
		Thread model: posix
		gcc version 4.7.3 (Ubuntu/Linaro 4.7.3-1ubuntu1) 
