# games

Game specific source code.

Together with the generated assembler files from the *SR-games* / *SRW-games* subproject, these files can be used to build Windows (x86/x64) or Linux (x86/x64/arm/arm64/riscv64) or MacOS (x64/arm64) versions of the games.

The games use [SDL](https://libsdl.org/ "Simple DirectMedia Layer") and [SDL_mixer](https://www.libsdl.org/projects/SDL_mixer/ "sample multi-channel audio mixer library") libraries.

The games can be built using [scons](http://scons.org/ "SCons: A software construction tool"). To control which version of the game should be built, use SCons Build Variables. Use `scons -h` to list available variables.

To play (or play better) music, use also midi plugins (and libraries) from *midi-plugins* subproject (and external open/closed source libraries).

To enhance the displayed image in DOS games, use also scaler plugins from *scaler-plugins* subproject.

The *release* subdirectories contain startup scripts and example configuration files.
