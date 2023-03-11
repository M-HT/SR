# midi-libs

Libraries that are used by plugins in *midi-plugins* subproject to play MIDI music.

## WildMidiA

WildMidiA is a library that's used in Albion to play background effects, that are played using MIDI (not General MIDI).

The directory *wildmidiA-0.2.3.5svn* contains the library, which is a modification of the [WildMidi](https://sourceforge.net/projects/wildmidi/ "WildMidi Midi Library and Player") library.

To compile:

* run `./bootstrap`
* on 32-bit linux:
  * run `./configure`
* on 64-bit linux (x64 / x86_64):
  * run `./configure --build=i686-pc-linux-gnu "CFLAGS=-m32" "LDFLAGS=-m32"`
* run `make`

## Other

Other libraries are available from their respective web pages:

* [WildMIDI](https://github.com/Mindwerks/wildmidi "WildMIDI: A Software Synthesizer")
* [BASSMIDI](https://www.un4seen.com/bass.html "BASS audio library")
* [libADLMIDI](https://github.com/Wohlstand/libADLMIDI/ "A Software MIDI Synthesizer library with OPL3 (YMF262) emulator")
