# midi-plugins

Plugins used by the games to play MIDI (and other types) music.

To compile the plugins run the *komp-x86.sh* script (or the *komp-arm.sh* script, if compiling on ARM).

## General plugins

**midi-wildmidi** plays General MIDI music using [WildMIDI](https://github.com/Mindwerks/wildmidi "WildMIDI: A Software Synthesizer") library. WildMIDI uses GUS sound patches to play MIDI music.

**midi-bassmidi** plays General MIDI music using [BASSMIDI](https://www.un4seen.com/bass.html "BASS audio library") library. BASSMIDI uses a soundfont (\*.sf2) to play MIDI music.

**midi-adlmidi** plays General MIDI music using [libADLMIDI](https://github.com/Wohlstand/libADLMIDI/ "A Software MIDI Synthesizer library with OPL3 (YMF262) emulator") library. libADLMIDI uses an OPL3 emulator to play MIDI music.

**midi2-windows** plays General MIDI and MT-32 music using [Windows API](https://learn.microsoft.com/en-us/windows/win32/multimedia/midi-reference "MIDI Reference"). Windows API can use hardware or software synth. The default software synth doesn't require additional files to play MIDI music.

**midi2-alsa** plays General MIDI and MT-32 music using [ALSA sequencer API](https://www.alsa-project.org/alsa-doc/alsa-lib/group___sequencer.html "MIDI Sequencer"). ALSA sequencer can use hardware or software synth (like [FluidSynth](https://www.fluidsynth.org/ "A SoundFont Synthesizer") or [TiMidity++](https://timidity.sourceforge.net/ "TiMidity++")).

## Game-specific plugins

### Albion

**midiA-wildmidi** plays MIDI (not General MIDI) background effects using WildMidiA library. WildMidiA is a modification of the [WildMidi](https://sourceforge.net/projects/wildmidi/ "WildMidi Midi Library and Player") library. The source is in the directory *wildmidiA-0.2.3.5svn* in *midi-libs* subproject.

### X-Com games (DOS versions)

These plugins use small x86 emulator to run the music driver from the game. The x86 emulator is not a complete x86 emulator. It supports only 64kB of RAM and implements only enough x86 instructions to run the music drivers.

**adlib-dosbox_opl** plays Adlib (OPL2) music using OPL2/OPL3 emulation library from [DOSBox](https://www.dosbox.com/ "DOS-emulator") or [Nuked OPL3 emulator](https://github.com/nukeykt/Nuked-OPL3 " Highly accurate Yamaha OPL3 (YMF262, CT1747) emulator") (sources are included). No additional files are required to play music.

**mt32-munt** plays MT-32 music using [Munt](https://munt.sourceforge.net/ "multi-platform software synthesiser") library (source is included). It requires Control and PCM roms from MT-32 or CM-32L (LAPC-I) to play music. CM-32L (LAPC-I) roms are preferred. The CM-32L (LAPC-I) roms filenames must be CM32L_CONTROL.ROM and CM32L_PCM.ROM. The MT-32 roms filenames must be MT32_CONTROL.ROM and MT32_PCM.ROM.
