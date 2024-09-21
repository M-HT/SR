Warcraft: Orcs & Humans for Linux (arm-aarch64)
Version 1.4.0

Original DOS Warcraft CD version is required for playing.
(version from GOG.com can be used for playing)

Libraries
---------
The game (SDL2 version) requires following 64-bit libraries: SDL2, SDL2_mixer
On debian based distributions these libraries are in following packages: libsdl2-2.0-0 libsdl2-mixer-2.0-0

The game (SDL1 version) requires following 64-bit libraries: SDL, SDL_mixer, OpenGL
On debian based distributions these libraries are in following packages: libsdl1.2debian libsdl-mixer1.2 libgl1

Some midi plugins may require additional libraries.


Installation
------------

Put files from this archive into the installed game's directory.

Simple instructions:
1) install Warcraft on your PC
2) copy the files from this archive into the game's directory
3) run the game using Warcraft.sh

Example (Detailed instructions):
1) install Warcraft on your PC
    - create directory ~/Games
    - if you have original DOS version then install Warcraft using DOSBox
      - in dosbox: mount ~/Games as C:
      - in dosbox: install Warcraft into C:\WARCRAFT
      - in dosbox: if necessary, then install patch(es) for Warcraft
    - if you have version from GOG.com then extract Warcraft from GOG.com windows installer
      - extract Warcraft using innoextract into ~/Games/WARCRAFT

2) copy the files from this archive into the game's directory
    - copy the content of this archive into ~/Games/WARCRAFT

3) run the game using Warcraft.sh
    - run Warcraft.sh in the game's directory: ~/Games/WARCRAFT/Warcraft.sh


Music
-----

The game's MIDI music can be played using one of following libraries:
ALSA sequencer, WildMIDI, BASSMIDI, libADLMIDI, SDL_mixer

libADLMIDI is the default library, others can be selected in the configuration file.
ALSA sequencer can use hardware or software synth (like Fluidsynth or TiMidity++).
libADLMIDI requires no additional files for MIDI playback,
WildMIDI requires GUS patches for MIDI playback,
BASSMIDI requires a soundfont for MIDI playback,
libADLMIDI uses OPL3 emulator for MIDI playback.

ALSA sequencer can detect usable synth automatically or it can be selected in the configuration file.

GUS patches can be installed anywhere, but the file timidity.cfg must be
either in the game's directory or in /etc/timidity/timidity.cfg
EawPats is a good sounding set of patches.

Soundfont (for BASSMIDI) can be either copied to the game's directory
or it can be stored anywhere, but the soundfont location must be written
in the configuration file.


Configuration
-------------

Configuration is stored in the file Warcraft.cfg.

Configuration can be changed either by editing the configuration file directly,
or by configuration editor (Warcraft-setup.sh).


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

HQX scaler uses neon instructions.
Nuked OPL3 emulator uses neon instructions.


Changes
-------

v1.4.0 (2024-09-21)
* add option to use higher quality audio resampling
* remove option to interpolate audio when resampling
* add support for higher sampling rates
* add support for using gamepad or joystick as mouse/keyboard
* use latest versions of libADLMIDI, WildMIDI libraries
* change setting volume in libADLMIDI
* modify ALSA midi plugin to try all ports with the same client number when looking for midi port based on client name
* minor bug fixes

v1.3.1 (2023-11-15)
* fix aspect ratio in fullscreen
* minor bug fixes

v1.3.0 (2023-08-13)
first Linux (arm-aarch64) version
