Albion for Linux (x86)
Version 1.7.4

Original Albion version 1.38 is required for playing.
(English version preferred, but other languages should also work)
(version from GOG.com can be used for playing)

Libraries
---------

The game (SDL1 version) requires following 32-bit libraries: SDL, SDL_mixer, OpenGL
On debian based distributions these libraries are in following packages: libsdl1.2debian:i386 libsdl-mixer1.2:i386 libgl1:i386

The game (SDL2 version) requires following 32-bit libraries: SDL2, SDL2_mixer
On debian based distributions these libraries are in following packages: libsdl2-2.0-0:i386 libsdl2-mixer-2.0-0:i386

Following optional 32-bit library is needed only to create screenshots in PNG format (selectable in configuration file): zlib
On debian based distributions this library is in following package: zlib1g:i386

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


Misc
----

WildMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
libADLMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
HQX scaler uses version 2.1 of the GNU Lesser General Public License (LGPLv2.1).
xBRZ scaler uses version 3 of the GNU General Public License (GPLv3).

Playback of background sound/music effects (played using music driver) uses modified WildMIDI library.

Source code is available on GitHub: https://github.com/M-HT/SR

MIDI playback using SDL_mixer can be selected in the configuration file, but
it sounds worse and can't play more than one MIDI stream simultaneously
(background sound/music effects (played using music driver) won't work).


Changes
-------

v1.7.4 (2023-11-18)
* fix aspect ratio in fullscreen
* fix bug in original code
* minor bug fixes


v1.7.3 (2023-03-26)
* optimize HQX and xBRZ scalers for more speed
* use more threads for advanced scaling (HQX and xBRZ)
* use latest versions of libADLMIDI, WildMIDI, BASSMIDI libraries
* add support for using Nuked OPL3 emulator in libADLMIDI
* fix moving mouse cursor outside the game area in fullscreen
* minor bug fixes


v1.7.2 (2022-01-30)
* ignore repeated keys when using SDL2
* fix Shift+Key ascii codes
* add support for inputing characters using Alt codes
* screen redraw 60 times per second
* minor bug fixes


v1.7.1 (2021-01-23)
* add options to switch WSAD and arrow keys
* fix audio when using SDL2
* fix rendering when using SDL2


v1.7.0 (2021-01-13)
* add advanced scaling options - nearest neighbour, scaler plugins: hqx, xbrz
* integrate video playing (intro and credits) into main executable
* change how fullscreen works
* add several options for making screenshots
* fix bug in the original code
* minor bug fixes


v1.6.1 (2018-01-01)
* add new MIDI plugin: adlmidi - it plays MIDI music using OPL3 emulator
  and also plays background sound/music effects
* fix for filesystems with 64-bit inodes
* modify ALSA midi plugin to select volume by changing volume of all channels instead of changing note velocity


v1.6.0 (2016-10-11)
* rework keyboard input for ASCII characters
* add keyboard input for language specific characters
  (works with English, French, German and Czech versions of Albion)
* MIDI playback using ALSA sequencer
  (ALSA sequencer can use hardware or software synth - like Fluidsynth or TiMidity++)
* add option to select the shape of SDL mouse cursor in window mode
  (normal cursor, minimal cursor or no cursor)
* minor bug fixes


v1.5.5 (2015-08-26)
* limit the maximum number of sold items to 99
* fix bug in the original code


v1.5.4 (2015-08-15)
* fix bug in the original code
* fix F11 and F12 keys
* add configuration option to select screenshot format


v1.5.1 (2015-01-03)
first Linux (x86) version
