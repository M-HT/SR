Warcraft: Orcs & Humans for Windows (x86)
Version 1.3.0

Original DOS Warcraft CD version is required for playing.
(version from GOG.com can be used for playing)

Installation
------------

Put files from this archive into the installed game's directory.

Simple instructions:
1) install Warcraft on your PC
2) copy the files from this archive into the game's directory
3) run the game using Warcraft.cmd or SR-War.exe

Example (Detailed instructions):
1) install Warcraft on your PC
    - create directory C:\Games
    - if you have original DOS version then install Warcraft using DOSBox
      - in dosbox: mount C:\Games as C:
      - in dosbox: install Warcraft into C:\WARCRAFT
      - in dosbox: if necessary, then install patch(es) for Warcraft
    - if you have version from GOG.com then install Warcraft using GOG.com windows installer
      - install Warcraft into C:\Games\WARCRAFT

2) copy the files from this archive into the game's directory
    - copy the content of this archive into C:\Games\WARCRAFT

3) run the game using Warcraft.cmd or SR-War.exe
    - run Warcraft.cmd or SR-War.exe in the game's directory: C:\Games\WARCRAFT\Warcraft.cmd


Music
-------------
The game's MIDI music can be played using one of following libraries:
NativeWindows, WildMIDI, BASSMIDI, libADLMIDI, SDL_mixer

libADLMIDI is the default library, others can be selected in the configuration file.
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


Configuration
-------------

Configuration is stored in the file Warcraft.cfg.


Misc
----

Multiplayer does not work (and it's not planned to make it work).

WildMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
libADLMIDI library uses version 3 of the GNU Lesser General Public License (LGPLv3).
HQX scaler uses version 2.1 of the GNU Lesser General Public License (LGPLv2.1).
xBRZ scaler uses version 3 of the GNU General Public License (GPLv3).

Source code is available on GitHub: https://github.com/M-HT/SR


Changes
-------

v1.3.0 (2023-08-13)
first Windows (x86) version
