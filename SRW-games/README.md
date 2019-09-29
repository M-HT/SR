# SRW-games

The information about the original Windows executables.

To generate an assembler version of the original executable, put the static recompiler (*SRW.exe*) and the original executable in the appropriate subdirectory and run *build-x86.sh* (or *build-llasm.sh*).

The generated assembler version consists of *.asm* and *.inc* files (or *.llasm* and *.llinc* files). Move these files into the appropriate subdirectory in the *games* subproject.

## Septerra Core

#### SRW

The original executable is *Septerra104.exe* from the English version 1.04.

* file size: 729088 bytes
* md5: 94daaeb19c17d9af3e2ae245219037a9
* sha1: 2dd27a46f41a91fed0e6ee7205e4b9cef61275c9
* sha256: f2f6040416589935ccbc206fee4db1ef369f6c9e8d167e631b190919b12fdf91

The generated files should be moved to *Septerra Core/SR-Septerra/x86* (or *llasm*) subdirectory in *games* subproject.

