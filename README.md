# SR

A project to [statically recompile](https://en.wikipedia.org/wiki/Binary_translation "Static binary translation") DOS game executables to create Windows or Linux (x86 or arm) versions of the games.

Following games are supported:

* [Albion](https://en.wikipedia.org/wiki/Albion_\(video_game\) "Albion")
* [X-Com: UFO Defense (UFO: Enemy Unknown)](https://en.wikipedia.org/wiki/UFO:_Enemy_Unknown "X-Com: UFO Defense (UFO: Enemy Unknown)")
* [X-Com: Terror from the Deep](https://en.wikipedia.org/wiki/X-COM:_Terror_from_the_Deep "X-Com: Terror from the Deep")
* [Warcraft: Orcs & Humans](https://en.wikipedia.org/wiki/Warcraft:_Orcs_%26_Humans "Warcraft: Orcs & Humans")

The source code is released with MIT license (except libraries, etc. by other people, which have their own license).
For the purpose of using the code in GPL projects, the source code is also released with GPLv2 or later and LGPLv2.1 or later.

<hr/>

The projects consists of following subprojects (read the readmes in subproject directories for more information):

* **SR**
  * The static recompiler itself.
  * It takes the original executable as an input together with information about the original executable and produces x86 or arm assembler version of the executable.
  * The generated x86/arm assembler version of the executable is not part of the project.
* **SR-games**
  * The information about the original executables.
* **games**
  * Game specific source code.
  * Together with the generated assembler versions of the executables, these files can be used to build Windows or Linux (x86 or arm) versions of the games.
  * Uses plugins to play (or play better) music.
* **midi-libs**
  * Libraries that are used by plugins in *midi-plugins* subproject to play MIDI music.
* **midi-plugins**
  * Plugins used by the games to play MIDI (and other types) music.
* **pycfg**
  * A configuration utility that can be used on Linux (sorry Windows users) to change settings in the configuration files (without editing the files).

<hr/>

Some notes:

* The project supports creating 32-bit, not 64-bit, Windows or Linux versions of the games.
* The generated arm version is little-endian, not big-endian.
* The generated arm version supports softfp and hardfp calling conventions.
