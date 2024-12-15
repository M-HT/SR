Battle Isle 3 for Linux (x86)
Version 1.0.5

Original Battle Isle 3 is required for playing.
(version from GOG.com can be used for playing)

Libraries
---------
The game requires Wine (https://www.winehq.org/) to run.


Installation
------------

Put files from this archive into the installed game's directory.
The game looks for the CDs in the DATA subdirectory (in installed game's directory).
If you have version from GOG.com, then the CDs are already copied there.
Otherwise you have to create the DATA subdirectory and copy the CDs there.
Alternatively you can copy the CDs elsewhere, but you need to edit the path to them in BattleIsle3.sh.
Run BattleIsle3.sh in the installed game's directory.


Music
-----

The game's MIDI music can be played using one of following libraries:
ALSA sequencer, WildMIDI, BASSMIDI, libADLMIDI

libADLMIDI is the default library, others can be selected in the configuration file.
ALSA sequencer can use hardware or software synth (like Fluidsynth or TiMidity++).
libADLMIDI requires no additional files for MIDI playback,
WildMIDI requires GUS patches for MIDI playback,
BASSMIDI requires a soundfont for MIDI playback,
libADLMIDI uses OPL3 emulator for MIDI playback.
Music can be played on MT-32 synth using ALSA sequencer.

ALSA sequencer can detect usable synth automatically or it can be selected in the configuration file.

GUS patches can be installed anywhere, but the file timidity.cfg must be
either in the game's directory or in /etc/timidity/timidity.cfg
EawPats is a good sounding set of patches.

Soundfont (for BASSMIDI) can be either copied to the game's directory
or it can be stored anywhere, but the soundfont location must be written
in the configuration file.


Configuration
-------------

Configuration is stored in the file BI3.cfg.
Paths to games files can be set in the file BattleIsle3.sh.


Misc
----

The game uses following libraries for playing video:
quicktime2 (http://libquicktime.sourceforge.net/)
ffmpeg (https://www.ffmpeg.org/)

The game optionally uses SpeexDSP library for audio resampling (https://github.com/xiph/speexdsp).

Source code is available on GitHub: https://github.com/M-HT/SR


Changes
-------

v1.0.5 (2024-12-15)
* add option to play music on MT-32 synth
* add french resource texts
* bug fixes


v1.0.4 (2024-09-15)
* add option to use higher quality midi resampling in libADLMIDI, WildMIDI, BASSMIDI libraries
* use latest versions of libADLMIDI, WildMIDI libraries
* change setting volume in libADLMIDI
* modify ALSA midi plugin to try all ports with the same client number when looking for midi port based on client name


v1.0.3 (2023-11-13)
* use bundled quicktime2 and ffmpeg libraries instead of system libraries
* fix reading command line options in path to executable
* minor bug fixes


v1.0.2 (2023-05-25)
* add option to change resolution
* add option to increase max window size


v1.0.1 (2023-03-26)
* use latest versions of libADLMIDI, WildMIDI, BASSMIDI libraries
* add support for using Nuked OPL3 emulator in libADLMIDI
* minor bug fixes


v1.0.0 (2021-06-27)
first Linux (x86) version
