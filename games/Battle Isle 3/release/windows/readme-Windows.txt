Battle Isle 3 for Windows (x86)
Version 1.0.2

Original Battle Isle 3 is required for playing.
(version from GOG.com can be used for playing)

Installation
------------

Install game.
Put files from this archive into the installed game's directory.
The game looks for the CDs in the DATA subdirectory (in installed game's directory).
If you have version from GOG.com, then the CDs are already copied there.
Otherwise you have to create the DATA subdirectory and copy the CDs there.
Alternatively you can copy the CDs elsewhere, but you need to edit the path to them in BattleIsle3.cmd.
Run BattleIsle3.cmd in the installed game's directory.


Music
-----

The game's MIDI music can be played using one of following libraries:
NativeWindows, WildMIDI, BASSMIDI, libADLMIDI, original

NativeWindows is the default library, others can be selected in the configuration file.
libADLMIDI, NativeWindows and original don't require additional files for MIDI playback,
WildMIDI requires GUS patches for MIDI playback,
BASSMIDI requires a soundfont for MIDI playback,
libADLMIDI uses OPL3 emulator for MIDI playback,
original uses Windows 3.1 compatible method for MIDI playback.

NativeWindows can use the default synth or it can be selected in the configuration file.

GUS patches can be installed anywhere, but the file timidity.cfg must be
either in the game's directory or in C:\TIMIDITY\timidity.cfg
EawPats is a good sounding set of patches.

Soundfont (for BASSMIDI) can be either copied to the game's directory
or it can be stored anywhere, but the soundfont location must be written
in the configuration file.


Configuration
-------------

Configuration is stored in the file BI3.cfg.
Paths to games files can be set in the file BattleIsle3.cmd.


Misc
----

The game uses following libraries for playing video:
quicktime2 (http://libquicktime.sourceforge.net/)
dlfcn-win32 (https://github.com/dlfcn-win32/dlfcn-win32)
ffmpeg (https://www.ffmpeg.org/)
intl from gettext (https://www.gnu.org/software/gettext/)

Source code is available on GitHub: https://github.com/M-HT/SR


Changes
-------

v1.0.2 (2023-05-25)
* add option to change resolution
* add option to increase max window size


v1.0.1 (2023-03-26)
* use latest versions of libADLMIDI, WildMIDI, BASSMIDI libraries
* add support for using Nuked OPL3 emulator in libADLMIDI
* minor bug fixes


v1.0.0 (2021-06-27)
first Windows (x86) version
