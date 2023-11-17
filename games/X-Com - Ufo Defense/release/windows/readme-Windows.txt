X-COM: UFO Defense for Windows (x86)
Version 1.5.1

Original DOS X-COM: UFO Defense version 1.4 is required for playing.
(Gold version for Windows should also work)
(version from GOG.com can be used for playing)

Installation
------------

Put files from this archive into the installed game's directory.

Simple instructions:
1) install X-COM: UFO Defense on your PC
2) copy the files from this archive into the game's directory
3) run the game using xcom1.cmd or SR-Xcom1.exe

Example (Detailed instructions):
1) install X-COM: UFO Defense on your PC
    - create directory C:\Games
    - if you have original DOS version then install X-COM: UFO Defense using DOSBox
      - in dosbox: mount C:\Games as C:
      - in dosbox: install X-COM: UFO Defense into C:\UFO
      - in dosbox: if necessary, then install patch(es) for X-COM
    - if you have version from GOG.com then install X-COM: UFO Defense using GOG.com windows installer
      - install X-COM: UFO Defense into C:\Games\UFO

2) copy the files from this archive into the game's directory
    - copy the content of this archive into C:\Games\UFO

3) run the game using xcom1.cmd or SR-Xcom1.exe
    - run xcom1.cmd or SR-Xcom1.exe in the game's directory: C:\Games\UFO\xcom1.cmd


Music
-----

The game can play General MIDI music (Windows and DOS game versions) or Adlib/MT-32 music (DOS game version).
The General MIDI music can be played using one of following libraries:
NativeWindows, WildMIDI, BASSMIDI, libADLMIDI, SDL_mixer

NativeWindows is the default library, others can be selected in the configuration file.
NativeWindows and libADLMIDI dond't require additional files for MIDI playback,
WildMIDI requires GUS patches for MIDI playback,
BASSMIDI requires a soundfont for MIDI playback,
libADLMIDI uses OPL3 emulator for MIDI playback.

NativeWindows can use the default synth or it can be selected in the configuration file.

GUS patches can be installed anywhere, but the file timidity.cfg must be
either in the game's directory or in C:\TIMIDITY\timidity.cfg
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


Changes
-------

v1.5.1 (2023-11-17)
* fix aspect ratio in fullscreen
* minor bug fixes

v1.5.0 (2023-08-12)
first Windows (x86) version
