# SR

The static recompiler (for DOS executables) itself.

This utility takes an original DOS executable and information about the original executable as an input and produces x86 / arm / llasm assembler version of the executable.

The generated assembler version requires additional files to compile and run. The non-specific files are in the *x86-support*, *arm-support* and *llasm-support* subdirectories.

The utility uses [Judy](http://judy.sourceforge.net/ "Judy Arrays Web Page") library and modified [udis86](http://udis86.sourceforge.net/ "Udis86 Disassembler Library for x86 / x86-64") library (version 1.6) -- the modified source is in the *udis86-1.6* subdirectory.
The current (2.15.05) version of [NASM](https://www.nasm.us/doc/) is not working with SR. Use an older version like 2.12. 

The utility can be built using [scons](http://scons.org/ "SCons: A software construction tool") -- the result is an executable file *SR.exe*.

By default a version that generates x86 assembler code is compiled. To compile a version that generates arm / llasm assembler code, change the definition of OUTPUT_TYPE from OUT_X86 to OUT_ARM_LINUX / OUT_LLASM in file *SR_defs.h*.

