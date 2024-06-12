# SRW

The static recompiler (for Windows executables) itself.

This utility takes an original Windows executable and information about the original executable as an input and produces x86 / llasm assembler version of the executable.

The generated assembler version requires additional files to compile and run. The non-specific files are in the *x86-support* subdirectory and in the *llasm-support* subdirectory of *SR* directory.

The utility uses [Judy](http://judy.sourceforge.net/ "Judy Arrays Web Page") library and [udis86](http://udis86.sourceforge.net/ "Udis86 Disassembler Library for x86 / x86-64") library (version 1.7.2) -- the source is in the *udis86-1.7.2* subdirectory.

The utility can be built using [scons](http://scons.org/ "SCons: A software construction tool") -- the result is an executable file *SRW.exe*.

Compilation works with gcc/clang compilers. Compatibility with other compilers (e.g. Visual Studio) is not guaranteed.

If you're not using prebuilt Judy library, but you're compiling it yourself, then you might try using [this fork](https://github.com/dlmiles/libjudy "Fork of the Judy C library") which contains some patches and bugfixes.

By default a version that generates x86 assembler code is compiled. To compile a version that generates llasm assembler code, change the definition of OUTPUT_TYPE from OUT_X86 to OUT_LLASM in file *SR_defs.h*.

