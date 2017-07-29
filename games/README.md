# games

Game specific source code.

Together with the generated assembler files from the *SR-games* subproject, these files can be used to build Windows or Linux (x86 or arm) versions of the games.

The games use [SDL](https://libsdl.org/ "Simple DirectMedia Layer"), [SDL_mixer](https://www.libsdl.org/projects/SDL_mixer/ "sample multi-channel audio mixer library") and OpenGL libraries.

The games can be built using [scons](http://scons.org/ "SCons: A software construction tool"). To control which version of the game should be built, use SCons Build Variables. Use `scons -h` to list available variables.

To play (or play better) music, use also midi plugins (and libraries) from *midi-plugins* subproject (and *midi-libs* subproject or external closed source libraries).

The *release* subdirectories contain startup scripts and example configuration files.
