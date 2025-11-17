# SRW

The static recompiler (for Windows executables) itself.

This utility takes an original Windows executable and information about the original executable as an input and produces x86 / llasm / x64 assembler version of the executable.

The generated assembler version requires additional files to compile and run. The non-specific files are in the *x86-support*, *llasm-support* and *x64-support* subdirectories of *SR* directory.

The utility uses files from [udis86](http://udis86.sourceforge.net/ "Udis86 Disassembler Library for x86 / x86-64") library (version 1.7.2) -- the source is in the *udis86-1.7.2* subdirectory.

The utility can be built using [scons](http://scons.org/ "SCons: A software construction tool") -- the result is an executable file *SRW.exe*.

The utility uses c++ containers by default or optionally [Judy](http://judy.sourceforge.net/ "Judy Arrays Web Page") library.

If you're not using prebuilt Judy library, but you're compiling it yourself, then you might try using [this fork](https://github.com/dlmiles/libjudy "Fork of the Judy C library") which contains some patches and bugfixes.

By default a version that generates x86 assembler code is compiled. To compile a version that generates llasm / x64 assembler code, change the definition of OUTPUT_TYPE from OUT_X86 to OUT_LLASM / OUT_X64 in file *SR_defs.h*.

