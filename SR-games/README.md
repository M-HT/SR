# SR-games

The information about the original executables.

To generate an assembler version of the original executable, put the static recompiler (*SR.exe*) and the original executable in the appropriate subdirectory and run *build-x86.sh* (or *build-arm.sh*).

The generated assembler version consists of *.asm* (or *.s*) and *.inc* files. Move these files into the appropriate subdirectory in the *games* subproject.

## Albion

#### SR

The original executable is *MAIN.EXE* from the English version 1.38.

* file size: 1109655 bytes
* md5: c4171d5eb0ebd36b132e081f8362a075
* sha1: e8f3a6df8909f8e6bc54bcced643cd4a6e2dd3cc
* sha256: 476227b0391cf3452166b7a1d52b012ccf6c86bc9e46886dafbed343e9140710

The generated files should be moved to *Albion/SR-Main/x86* (or *arm*) subdirectory in *games* subproject.

## X-Com: UFO Defense (UFO: Enemy Unknown)

#### SR1

The original executable is *GEOSCAPE.EXE* from the DOS English version 1.4.

* file size: 382957 bytes
* md5: 33ea0819e6888f0450f9a1e5e19dc98b
* sha1: 832d78f3c9a9c50960f10473592ae2c48dc7b6a7
* sha256: 6dff79b532e1da687e30624ba7b2a1215433467fcd9061415d498c9998df61d3

The generated files should be moved to *X-Com - Ufo Defense/SR-Geoscape/x86* (or *arm*) subdirectory in *games* subproject.

#### SR2

The original executable is *TACTICAL.EXE* from the DOS English version 1.4.

* file size: 384985 bytes
* md5: 0b7187a12c042466e7043d070e1eaef5
* sha1: bb6a845cc577213ed8ad57e4e4b576222d218f45
* sha256: e9e3a6dcc52598a07903050ee2697b71204df9d04eb71f8559aa7e163967f797

The generated files should be moved to *X-Com - Ufo Defense/SR-Tactical/x86* (or *arm*) subdirectory in *games* subproject.

#### SR3

The original executable is *INTRO.EXE* from the DOS English version 1.4.

* file size: 74314 bytes
* md5: 28098b9411d9454ad6676d99cefc0e4b
* sha1: 927f0896dc91eea3b948ad60ed71870843ce5ad0
* sha256: 513569ceea82ee0b265cad3605b82324ac1adcd98adbff7f6ce0fbef7af4da30

The generated files should be moved to *X-Com - Ufo Defense/SR-Intro/x86* (or *arm*) subdirectory in *games* subproject.

## X-Com: Terror from the Deep

#### SR1

The original executable is *GEOSCAPE.EXE* from the DOS English version 2.1.

* file size: 496057 bytes
* md5: 9befee7c31665db277660092caf4f6cf
* sha1: 898636a3b4c7b11266a1fbf4c7a4467e6ec412a9
* sha256: 02f1a492bf237333e795623502e4fb5cb88fa77d54f567fec7c905e6b390c2a7

The generated files should be moved to *X-Com - Terror from the Deep/SR-Geoscape/x86* (or *arm*) subdirectory in *games* subproject.

#### SR2

The original executable is *TACTICAL.EXE* from the DOS English version 2.1.

* file size: 410777 bytes
* md5: 12be97f36f82b3c84258196a9ef34e78
* sha1: 61130d090b84eb3263dd30d65090077b2f648479
* sha256: c3b340e94aee2f3690eaade4cd990e3c8ba0bc9c5b6b1d122f28ae4eef24ca8c

The generated files should be moved to *X-Com - Terror from the Deep/SR-Tactical/x86* (or *arm*) subdirectory in *games* subproject.

## Warcraft: Orcs & Humans

#### SR

The original executable is *WAR.EXE* from the DOS Floppy English version 1.21.

* file size: 369303 bytes
* md5: 9ffecab1a27fcca3c4a0247bf791caf6
* sha1: 656aeb0d2d1384ad4442e8d63911701ba5885e81
* sha256: 43c07a258cb3044a7e39211830c837567e17201a494718fab678b17253b77639

The generated files should be moved to *Warcraft/SR-War/x86* (or *arm*) subdirectory in *games* subproject.

#### SR-cd

The original executable is *WAR.EXE* from the DOS CD English version 1.21.

* file size: 320639 bytes
* md5: 04dfed1f0d0d3d7c1bb544ccb9278c76
* sha1: 35761741340cd8fb64caadd8eb7565e65d779b33
* sha256: aac8ae8d2a9e27039851147433d3365a61d7633acf003fc409ef5e5df5434dd5

The generated *.inc* files should be moved to *Warcraft/SR-War/x86/cdver* (or *arm/cdver*) subdirectory in *games* subproject.

The generated *.asm* (or *.s*) files should be modified (change the path to *.inc* files) according to existing files and then moved to *Warcraft/SR-War/x86* (or *arm*) subdirectory in *games* subproject.

#### SR-cd-gog

The original executable is *WAR.EXE* from the DOS CD English version 1.22 (from GOG.com).

* file size: 319291 bytes
* md5: 03c8e5bff6772eca8d4be8a22c243fd2
* sha1: b32ef7d87d1e51f3f0b20ce7911053f32a0d509a
* sha256: 1e966220bf09ab007c49c5ea5ebc42eb494f5cf2f74f894d3969b6115df8d8a9

The generated *.inc* files should be moved to *Warcraft/SR-War/x86/cdgogver* (or *arm/cdgogver*) subdirectory in *games* subproject.

The generated *.asm* (or *.s*) files should be modified (change the path to *.inc* files) according to existing files and then moved to *Warcraft/SR-War/x86* (or *arm*) subdirectory in *games* subproject.

