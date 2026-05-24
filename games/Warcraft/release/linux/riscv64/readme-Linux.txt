Warcraft: Orcs & Humans for Linux (riscv64)
Version 1.4.2

Original DOS Warcraft CD version is required for playing.
(version from GOG.com can be used for playing)

Libraries
---------

The game requires following 64-bit libraries: SDL2, SDL2_mixer
On debian based distributions these libraries are in following packages: libsdl2-2.0-0 libsdl2-mixer-2.0-0

Some midi plugins may require additional libraries.


Installation
------------

Put files from this archive into the installed game's directory.

Simple instructions:
1) install Warcraft on your PC (or copy installed game from another computer)
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
ALSA sequencer, WildMIDI, libADLMIDI, SDL_mixer

libADLMIDI is the default library, others can be selected in the configuration file.
ALSA sequencer can use hardware or software synth (like Fluidsynth or TiMidity++).
libADLMIDI requires no additional files for MIDI playback,
WildMIDI requires GUS patches for MIDI playback,
libADLMIDI uses OPL3 emulator for MIDI playback.
Music can be played on MT-32 synth using ALSA sequencer.

ALSA sequencer can detect usable synth automatically or it can be selected in the configuration file.

GUS patches can be installed anywhere, but the file timidity.cfg must be
either in the game's directory or in /etc/timidity/timidity.cfg
EawPats is a good sounding set of patches.


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


Changes
-------

v1.4.2 (2026-0?-??)
first Linux (riscv64) version
