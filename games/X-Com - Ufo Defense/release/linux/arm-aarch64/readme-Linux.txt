X-COM: UFO Defense for Linux (arm-aarch64)
Version 1.5.1

Original DOS X-COM: UFO Defense version 1.4 is required for playing.
(Gold version for Windows should also work)
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
1) install X-COM: UFO Defense on your PC
2) copy the files from this archive into the game's directory
3) run the game using xcom1.sh

Example (Detailed instructions):
1) install X-COM: UFO Defense on your PC
    - create directory ~/Games
    - if you have original DOS version then install X-COM: UFO Defense using DOSBox
      - in dosbox: mount ~/Games as C:
      - in dosbox: install X-COM: UFO Defense into C:\UFO
      - in dosbox: if necessary, then install patch(es) for X-COM
    - if you have version from GOG.com then extract X-COM: UFO Defense from GOG.com windows installer
      - extract X-COM: UFO Defense using innoextract into ~/Games/UFO

2) copy the files from this archive into the game's directory
    - copy the content of this archive into ~/Games/UFO

3) run the game using xcom1.sh
    - run xcom1.sh in the game's directory: ~/Games/UFO/xcom1.sh


Music
-----

The game can play General MIDI music (Windows and DOS game versions) or Adlib/MT-32 music (DOS game version).
The General MIDI music can be played using one of following libraries:
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

Adlib music can be played without additional files.
MT-32 music can be played using MUNT emulator or ALSA sequencer.
MUNT emulator requires Control and PCM roms from MT-32 or CM-32L/LAPC-I.


Configuration
-------------

Configuration is stored in the file Ufo.cfg.

Configuration can be changed either by editing the configuration file directly,
or by configuration editor (xcom1-setup.sh).


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

Source code is available on GitHub: https://github.com/M-HT/SR

HQX scaler uses neon instructions.
Nuked OPL3 emulator uses neon instructions.


Changes
-------

v1.5.1 (2023-11-17)
* fix aspect ratio in fullscreen
* optimize floating point instructions
* minor bug fixes

v1.5.0 (2023-08-12)
first Linux (arm-aarch64) version
