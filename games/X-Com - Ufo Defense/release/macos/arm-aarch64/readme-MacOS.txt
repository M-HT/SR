X-COM: UFO Defense for MacOS (arm-aarch64)
Version 1.6.2

Original DOS X-COM: UFO Defense version 1.4 is required for playing.
(Gold version for Windows should also work)
(version from GOG.com can be used for playing)

Game path
---------

Game path must be selected before launching the game.
Selected path must contain a directory "GEODATA" and a directory "UFOGRAPH".


Music
-----

The game can play General MIDI music (Windows and DOS game versions) or Adlib/MT-32 music (DOS game version).
The General MIDI music can be played using one of following libraries:
CoreMIDI, WildMIDI, BASSMIDI, libADLMIDI, SDL_mixer

libADLMIDI is the default library, others can be selected in the configuration file.
CoreMIDI can use hardware or software synth.
libADLMIDI requires no additional files for MIDI playback,
WildMIDI requires GUS patches for MIDI playback,
BASSMIDI requires a soundfont for MIDI playback,
libADLMIDI uses OPL3 emulator for MIDI playback.
General MIDI music can be played using AWE32 emulator (DOS game version).
Music can be played on MT-32 synth using CoreMIDI (MT-32 music in DOS game version / General MIDI music in Gold edition).

CoreMIDI can detect usable synth automatically or it can be selected in the configuration file.

GUS patches can be installed anywhere, but the file timidity.cfg must be
either in SR-Xcom1.app/Contents/Resources directory or in one of these locations:
 /etc/timidity/timidity.cfg
 /etc/timidity.cfg
 /usr/share/timidity/timidity.cfg
 /usr/local/lib/timidity/timidity.cfg
EawPats is a good sounding set of patches.

Soundfont (for BASSMIDI) can be either copied to SR-Xcom1.app/Contents/Resources directory
or it can be stored anywhere, but the soundfont location must be written
in the configuration file.

Adlib music can be played without additional files.
MT-32 music can be played using MUNT emulator or CoreMIDI.
MUNT emulator requires Control and PCM roms from MT-32 or CM-32L/LAPC-I.
AWE32 emulator requires AWE32 rom.


Configuration
-------------

Configuration is stored in the file Ufo.cfg.


Controls
--------

The game can use gamepad or joystick to emulate mouse and keyboard, if it's enabled in the configuration file.

Controller mappings can be added using file gamecontrollerdb.txt.

Gamepad / joystick controls:
Analog stick/pad: mouse movement
Digital stick/pad: mouse movement
A/X / 1st joystick button: left mouse button
B/Y / 2nd joystick button: right mouse button
Start / 3rd joystick button: virtual keyboard
Back / 4th joystick button: enter key


Misc
----

The executable contains fix for the "Difficulty bug".

WildMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
libADLMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
HQX scaler uses version 2.1 of the GNU Lesser General Public License (LGPLv2.1).
xBRZ scaler uses version 3 of the GNU General Public License (GPLv3).

Adlib music is played using either 'compat' OPL emulator from DOSBox or Nuked OPL3 emulator.
DOSBox 'compat' OPL emulator uses version 2.1 or later of the GNU Lesser General Public License (LGPLv2.1).
Nuked OPL3 emulator uses version 2.1 or later of the GNU Lesser General Public License (LGPLv2.1).

MUNT emulator uses version 2.1 or later of the GNU Lesser General Public License (LGPLv2.1).
AWE32 emulator uses version 2 of the GNU General Public License (GPLv2).

The game optionally uses SpeexDSP library for audio resampling (https://github.com/xiph/speexdsp).

Source code is available on GitHub: https://github.com/M-HT/SR


Changes
-------

v1.6.2 (2026-05-25)
first MacOS (arm-aarch64) version
