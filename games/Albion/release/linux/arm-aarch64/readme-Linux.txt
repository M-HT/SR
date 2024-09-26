Albion for Linux (arm-aarch64)
Version 1.8.0

Original Albion version 1.38 is required for playing.
(English version preferred, but other languages should also work)
(version from GOG.com can be used for playing)

Libraries
---------

The game (SDL1 version) requires following 64-bit libraries: SDL, SDL_mixer, OpenGL
On debian based distributions these libraries are in following packages: libsdl1.2debian libsdl-mixer1.2 libgl1

The game (SDL2 version) requires following 64-bit libraries: SDL2, SDL2_mixer
On debian based distributions these libraries are in following packages: libsdl2-2.0-0 libsdl2-mixer-2.0-0

Following optional 64-bit library is needed only to create screenshots in PNG format (selectable in configuration file): zlib
On debian based distributions this library is in following package: zlib1g

Some midi plugins may require additional libraries.


Installation
------------

Put files from this archive into the installed game's directory.

Simple instructions:
1) install Albion on your PC
2) copy the ALBION subdirectory from CD onto harddisk (into the Albion directory tree)
3) in file SETUP.INI change the path (SOURCE_PATH) from CD to the copied directory on disk (using relative path)
4) copy the files from this archive into the game's directory
5) run the game using Albion.sh

Example (Detailed instructions):
1) install Albion on your PC
    - create directory ~/Games
    - if you have original DOS version then install Albion using DOSBox
      - in dosbox: mount ~/Games as C:, mount cdrom as D:
      - in dosbox: install Albion into C:\ALBION
    - if you have version from GOG.com then extract Albion from GOG.com windows installer
      - extract Albion using innoextract (tested with version 1.4) into ~/Games/ALBION
      - move everything from ~/Games/ALBION/app into ~/Games/ALBION

2) copy the ALBION subdirectory from CD onto harddisk (into the Albion directory tree)
    - create directory ~/Games/ALBION/ALBIONCD (that's C:\ALBION\ALBIONCD in dosbox)
      (directory name must be DOS name - uppercase, at most 8 characters, no spaces, ...)
    - if you have version from GOG.com then the file game.gog is the image of the CD in BIN format
      - you can mount it using gCDEmu for example
    - copy the content of directory cdrom:/ALBION into ~/Games/ALBION/ALBIONCD

3) in file SETUP.INI change the path (SOURCE_PATH) from CD to the copied directory on disk (using relative path)
    - in file ~/Games/ALBION/SETUP.INI find line which begins with SOURCE_PATH=
    - replace it with line SOURCE_PATH=ALBIONCD\

4) copy the files from this archive into the game's directory
    - copy the content of this archive into ~/Games/ALBION

5) run the game using Albion.sh
    - run Albion.sh in the game's directory: ~/Games/ALBION/Albion.sh


Music
-----

The game's MIDI music can be played using one of following libraries:
ALSA sequencer, WildMIDI, BASSMIDI, libADLMIDI, SDL_mixer

libADLMIDI is the default library, others can be selected in the configuration file.
ALSA sequencer can use hardware or software synth (like Fluidsynth or TiMidity++).
libADLMIDI requires no additional files for MIDI playback,
WildMIDI and SDL_mixer require GUS patches for MIDI playback,
BASSMIDI requires a soundfont for MIDI playback,
libADLMIDI uses OPL3 emulator for MIDI playback.

ALSA sequencer can detect usable synth automatically or it can be selected in the configuration file.

GUS patches can be installed anywhere, but the file timidity.cfg must be
either in the game's directory or in /etc/timidity/timidity.cfg
EawPats is a good sounding set of patches.

Soundfont (for BASSMIDI) can be either copied to the game's directory
or it can be stored anywhere, but the soundfont location must be written
in the configuration file.


Enhanced 3D rendering
---------------------

Enhanced 3D rendering renders the 3d part of the game in double (or more) resolution,
instead of rendering it in the original resolution and then scaling it.
Rendering in more than double resolution is only available in advanced scaling mode.
There are some minor issues:
 - it works best when the 3D window size is set to 100%
 - when toggling scaling or enhanced 3D rendering and the game is in 3D mode
   and a message, dialog or popup menu is displayed, the 3D part may not be displayed correctly
   (until the message, dialog or popup menu is closed)
Enhanced 3D rendering can be turned off in the configuration file (or using the configurator).


Configuration
-------------

Configuration is stored in the file Albion.cfg.

Configuration can be changed either by editing the configuration file directly,
or by configuration editor (Albion-setup.sh).


Controls
--------

The game can use gamepad or joystick to emulate mouse and keyboard, if it's enabled in the configuration file.

Controller mappings can be added using file gamecontrollerdb.txt.

Gamepad / joystick controls:
Analog stick/pad: mouse movement
Digital stick/pad: up/down/left/right arrow key
A/X / 1st joystick button: left mouse button
B/Y / 2nd joystick button: right mouse button
Start / 3rd joystick button: virtual keyboard
Back / 4th joystick button: tab key
Left shoulder button: alt key
Right shoulder button: ctrl key


Misc
----

WildMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
libADLMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
HQX scaler uses version 2.1 of the GNU Lesser General Public License (LGPLv2.1).
xBRZ scaler uses version 3 of the GNU General Public License (GPLv3).

The game optionally uses SpeexDSP library for audio resampling (https://github.com/xiph/speexdsp).

Source code is available on GitHub: https://github.com/M-HT/SR

HQX scaler uses neon instructions.


Changes
-------

v1.8.0 (2024-09-26)
* add option to use higher quality audio resampling
* remove option to interpolate audio when resampling
* add support for higher sampling rates
* add support for using gamepad or joystick as mouse/keyboard
* use latest versions of libADLMIDI, WildMIDI libraries
* change setting volume in libADLMIDI
* modify ALSA midi plugin to try all ports with the same client number when looking for midi port based on client name
* minor bug fixes


v1.7.4 (2023-11-18)
* fix aspect ratio in fullscreen
* fix bug in original code
* minor bug fixes


v1.7.3a (2023-05-21)
* first Linux (arm-aarch64) version
