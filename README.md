# SR

A project to [statically recompile](https://en.wikipedia.org/wiki/Binary_translation "Static binary translation") DOS and Windows game executables to create Windows or Linux (x86 or arm) versions of the games.

Following DOS games are supported:

* [Albion](https://en.wikipedia.org/wiki/Albion_\(video_game\) "Albion")
* [X-Com: UFO Defense (UFO: Enemy Unknown)](https://en.wikipedia.org/wiki/UFO:_Enemy_Unknown "X-Com: UFO Defense (UFO: Enemy Unknown)")
* [X-Com: Terror from the Deep](https://en.wikipedia.org/wiki/X-COM:_Terror_from_the_Deep "X-Com: Terror from the Deep")
* [Warcraft: Orcs & Humans](https://en.wikipedia.org/wiki/Warcraft:_Orcs_%26_Humans "Warcraft: Orcs & Humans")

Following Windows games are supported:

* [Septerra Core: Legacy of the Creator](https://en.wikipedia.org/wiki/Septerra_Core "Septerra Core: Legacy of the Creator")
* [Battle Isle 3: Shadow of the Emperor (Battle Isle 2220: Shadow of the Emperor)](https://en.wikipedia.org/wiki/Battle_Isle_2220 "Battle Isle 3: Shadow of the Emperor (Battle Isle 2220: Shadow of the Emperor)")

The source code is released with MIT license (except libraries, etc. by other people, which have their own license).
For the purpose of using the code in GPL projects, the source code is also released with GPLv2 or later and LGPLv2.1 or later.

<hr/>

The projects consists of following subprojects (read the readme files in subproject directories for more information):

* **SR**
  * The static recompiler (for DOS executables) itself.
  * It takes the original executable as an input together with information about the original executable and produces x86 / arm / llasm / x64 assembler version of the executable.
  * The generated x86/arm/llasm/x64 assembler version of the executable is not part of the project.
* **SRW**
  * The static recompiler (for Windows executables) itself.
  * It takes the original executable as an input together with information about the original executable and produces x86 / llasm / x64 assembler version of the executable.
  * The generated x86/llasm/x64 assembler version of the executable is not part of the project.
* **llasm**
  * Program which converts *.llasm* file to [LLVM](https://llvm.org/ "LLVM") language-independent intermediate representation, which can be compiled to native code.
* **SR-games**
  * The information about the original DOS executables.
* **SRW-games**
  * The information about the original Windows executables.
* **games**
  * Game specific source code.
  * Together with the generated assembler versions of the executables, these files can be used to build Windows or Linux (x86 or arm) versions of the games.
  * Uses plugins to play (or play better) music.
* **midi-plugins**
  * Plugins used by the games to play MIDI (and other types) music.
* **scaler-plugins**
  * Plugins used by the DOS games to enlarge/enhance the displayed image.
* **pycfg**
  * A configuration utility that can be used on Linux (sorry Windows users) to change settings in the configuration files (without editing the files).
* **SR64-loader**
  * Loader for 64-bit versions.

<hr/>

Some notes:

* The project supports creating 32-bit, Windows or Linux versions of the games.
* The generated arm version is little-endian, not big-endian.
* The generated arm version supports softfp and hardfp calling conventions.
* Creating 64-bit versions is experimental.
  * Only Linux (x64, arm64) versions of Albion, Septerra Core, X-COM 1,2 and Warcraft are working at the moment.
  * 64-bit versions require a loader.
