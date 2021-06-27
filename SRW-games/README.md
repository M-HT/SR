# SRW-games

The information about the original Windows executables.

To generate an assembler version of the original executable, put the static recompiler (*SRW.exe*) and the original executable in the appropriate subdirectory and run *build-x86.sh* (or *build-llasm.sh*).

[NASM](https://www.nasm.us/) (Netwide Assembler) is needed to generate (and to compile) the x86 assembler version. Versions 2.15.03 -- 2.15.05 contain a bug which prevents them from working with the static recompiler. Use an earlier (or later) version.

The generated assembler version consists of *.asm* and *.inc* files (or *.llasm* and *.llinc* files). Move these files into the appropriate subdirectory in the *games* subproject.

## Septerra Core

#### SRW

The original executable is *Septerra104.exe* from the English version 1.04.

* file size: 729088 bytes
* md5: 94daaeb19c17d9af3e2ae245219037a9
* sha1: 2dd27a46f41a91fed0e6ee7205e4b9cef61275c9
* sha256: f2f6040416589935ccbc206fee4db1ef369f6c9e8d167e631b190919b12fdf91

The generated files should be moved to *Septerra Core/SR-Septerra/x86* (or *llasm*) subdirectory in *games* subproject.

## Battle Isle 3

#### SRW-ms_figtr

The original executable is *MS_FIGTR.DLL* from the English version.

* file size: 39936 bytes
* md5: f5f46e6841dce6fc92eb07d1743eec7a
* sha1: 160ed7fe442bc8a37eff0a36220a177cd9c6ebb1
* sha256: e9e69b1fb8e18d546f769204303a046dbe083e19ad1280a67bd40723b9e284e0

The generated files should be moved to *Battle Isle 3/SR-BI3/x86/ms_figtr* (or *llasm/ms_figtr*) subdirectory in *games* subproject.

#### SRW-sdi_1r

The original executable is *SDI_1R.EXE* from the English version.

* file size: 774144 bytes
* md5: a7778d5f4d1c9fd8017d8da80c85bb44
* sha1: ca6322a80552161fb61ba40a3955af4d166359be
* sha256: 1b6d69b54548588a37eb5919193f8ab01328208c91822079a77e6f42d2b34f51

Also accepted executable is *SDI_1R.EXE* from the English version (from GOG.com).

* file size: 774144 bytes
* md5: c0ea37923b18e67fa982e63319337dc2
* sha1: 6d861add8c93c6ccb1a5b6438a646e4be2df085c
* sha256: 03c64aba3bd305cefcf947795afbdb3b3a58f1598517ebdb15cc0fdfe63f438a

The generated files should be moved to *Battle Isle 3/SR-BI3/x86/sdi_1r* (or *llasm/sdi_1r*) subdirectory in *games* subproject.

#### SRW-wc_figtr

The original executable is *WC_FIGTR.DLL* from the English version.

* file size: 463939 bytes
* md5: a3a416cc07c6590c4865e2b76ea0c240
* sha1: 94572b4e599a91164c03a1ae9e5e0456bb30cd14
* sha256: f92da9d96138115c308fb096f16ed12c900e7832befa4f8e4425014d2cff6e44

The generated files should be moved to *Battle Isle 3/SR-BI3/x86/wc_figtr* (or *llasm/wc_figtr*) subdirectory in *games* subproject.

