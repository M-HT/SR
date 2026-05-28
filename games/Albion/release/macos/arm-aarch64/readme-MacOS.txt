Albion for MacOS (arm-aarch64)
Version 1.8.2

Original Albion version 1.38 is required for playing.
(English version preferred, but other languages should also work)
(version from GOG.com can be used for playing)

Game path
---------

Game path must be selected before launching the game.
Selected path must contain files "ALBION.EXE" and "SETUP.INI".
Files from ALBION subdirectory on the CD must be in a directory in the game directory tree.
In file SETUP.INI the path (SOURCE_PATH) must point (using relative path) to this directory.
Example:
Game path contains directory ALBIONCD which contains files from ALBION subdirectory on the CD.
File SETUP.INI contains following line: SOURCE_PATH=ALBIONCD\


Music
-----

The game's MIDI music can be played using one of following libraries:
CoreMIDI, WildMIDI, BASSMIDI, libADLMIDI, SDL_mixer

libADLMIDI is the default library, others can be selected in the configuration file.
CoreMIDI can use hardware or software synth.
libADLMIDI requires no additional files for MIDI playback,
WildMIDI requires GUS patches for MIDI playback,
BASSMIDI requires a soundfont for MIDI playback,
libADLMIDI uses OPL3 emulator for MIDI playback.
Music can be played on MT-32 synth using CoreMIDI.

CoreMIDI can detect usable synth automatically or it can be selected in the configuration file.

GUS patches can be installed anywhere, but the file timidity.cfg must be
either in SR-Albion.app/Contents/Resources directory or in one of these locations:
 /etc/timidity/timidity.cfg
 /etc/timidity.cfg
 /usr/share/timidity/timidity.cfg
 /usr/local/lib/timidity/timidity.cfg
EawPats is a good sounding set of patches.

Soundfont (for BASSMIDI) can be either copied to SR-Albion.app/Contents/Resources directory
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


Changes
-------

v1.8.2 (2026-05-28)
* first MacOS (arm-aarch64) version
