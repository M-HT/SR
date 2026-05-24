Warcraft: Orcs & Humans for MacOS (arm-aarch64)
Version 1.4.2

Original DOS Warcraft CD version is required for playing.
(version from GOG.com can be used for playing)

Game path
---------

Game path must be selected before launching the game.
Selected path must contain a directory "DATA" and a file "WAR.EXE".


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
either in SR-Warcraft.app/Contents/Resources directory or in one of these locations:
 /etc/timidity/timidity.cfg
 /etc/timidity.cfg
 /usr/share/timidity/timidity.cfg
 /usr/local/lib/timidity/timidity.cfg
EawPats is a good sounding set of patches.

Soundfont (for BASSMIDI) can be either copied to SR-Warcraft.app/Contents/Resources directory
or it can be stored anywhere, but the soundfont location must be written
in the configuration file.


Configuration
-------------

Configuration is stored in the file Warcraft.cfg.


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
Back / 4th joystick button: enter key
Left shoulder button: shift key
Right shoulder button: ctrl key (mouse helper disabled) / middle mouse button (mouse helper enabled)


Misc
----

Multiplayer does not work (and it's not planned to make it work).

WildMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
libADLMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
HQX scaler uses version 2.1 of the GNU Lesser General Public License (LGPLv2.1).
xBRZ scaler uses version 3 of the GNU General Public License (GPLv3).

The game optionally uses SpeexDSP library for audio resampling (https://github.com/xiph/speexdsp).

Source code is available on GitHub: https://github.com/M-HT/SR


Changes
-------

v1.4.2 (2026-05-24)
first MacOS (arm-aarch64) version
