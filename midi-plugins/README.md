# midi-plugins

Plugins used by the games to play MIDI (and other types) music.

To compile the pugins run the *komp-x86.sh* script.

## General plugins

**midi-wildmidi** plays MIDI music using [WildMidi](https://sourceforge.net/projects/wildmidi/ "WildMidi Midi Library and Player") library. WildMidi uses GUS sound patches to play MIDI music. The modified source is in the directory *wildmidi-0.2.3.5svn* in *midi-libs* subproject.

**midi-bassmidi** plays MIDI music using [BASSMIDI](http://www.un4seen.com/bass.html "BASS audio library") library. BASSMIDI uses soundfont (*.sf2*) to play MIDI music.

**midi2-windows** plays MIDI music using [Windows API](https://msdn.microsoft.com/en-us/library/vs/alm/dd757277\(v=vs.85\).aspx "MIDI Reference"). Windows API can use hardware or software synth. The default software synth doesn't require additional files to play MIDI music.

**midi2-alsa** plays MIDI music using [ALSA sequencer API](http://www.alsa-project.org/alsa-doc/alsa-lib/group___sequencer.html "MIDI Sequencer"). ALSA sequencer can use hardware or software synth (like [FluidSynth](http://www.fluidsynth.org/ "A SoundFont Synthesizer") or [TiMidity++](http://timidity.sourceforge.net/ "TiMidity++")).

## Game-specific plugins

### Albion

**midiA-wildmidi** plays MIDI (not General MIDI) background effects using WildMidiA library. WildMidiA is a modification of the WildMidi library. The source is in the directory *wildmidiA-0.2.3.5svn* in *midi-libs* subproject.

### X-Com games (DOS versions)

These plugins use small x86 emulator to run the music driver from the game. The x86 emulator is not a complete x86 emulator. It supports only 64kB of RAM and implements only enough x86 instructions to run the music drivers.

**adlib-dosbox_opl** plays Adlib music using OPL2/OPL3 emulation library from [DOSBox](http://www.dosbox.com/ "DOS-emulator") (source is included). No additional files are required to play music.

**mt32-munt** plays MT-32 music using [Munt](http://munt.sourceforge.net/ "multi-platform software synthesiser") library (source is included). It requires Control and PCM roms from MT-32 or CM-32L (LAPC-I) to play music. CM-32L (LAPC-I) roms are preferred. The CM-32L (LAPC-I) roms filenames must be CM32L_CONTROL.ROM and CM32L_PCM.ROM. The MT-32 roms filenames must be MT32_CONTROL.ROM and MT32_PCM.ROM.
