#!/bin/sh

# to enable using SpeexDSP either run the script with argument USE_SPEEXDSP or define non-empty environment variable USE_SPEEXDSP
for i in "$@"; do
  case $i in
    USE_SPEEXDSP)
      USE_SPEEXDSP=1
      shift
      ;;
    *)
      echo "Unknown argument: $i"
      shift
      ;;
  esac
done

if [ "x$USE_SPEEXDSP" != "x" ]
then
  SPEEXDSP_COMPILE=-DUSE_SPEEXDSP_RESAMPLER
  SPEEXDSP_LINK=-lspeexdsp
else
  SPEEXDSP_COMPILE=
  SPEEXDSP_LINK=
fi

cc -shared -Wl,-soname,midi-wildmidi.so -o midi-wildmidi.so -fpic -fvisibility=hidden -O2 -Wall -march=rv64gc $SPEEXDSP_COMPILE midi-wildmidi.c -I`pwd`/include -lWildMidi $SPEEXDSP_LINK -L`pwd`/lib/riscv64
#cc -shared -Wl,-soname,midi-bassmidi.so -o midi-bassmidi.so -fpic -fvisibility=hidden -O2 -Wall -march=rv64gc midi-bassmidi.c -I`pwd`/include -lbassmidi -lbass -L`pwd`/lib/riscv64
cc -shared -Wl,-soname,midi-adlmidi.so -o midi-adlmidi.so -fpic -fvisibility=hidden -O2 -Wall -march=rv64gc $SPEEXDSP_COMPILE midi-adlmidi.c -I`pwd`/include -lADLMIDI $SPEEXDSP_LINK -L`pwd`/lib/riscv64
cc -shared -Wl,-soname,midi2-alsa.so -o midi2-alsa.so -fpic -fvisibility=hidden -O2 -Wall -march=rv64gc midi2-alsa.c -lasound -lpthread

cd xcom12

CURDIR=`pwd`
cc -c -fpic -fvisibility=hidden -O2 -Wall -march=rv64gc mpsmusic-driver.c -I$CURDIR/include -I../
cd src
cc -c -fpic -fvisibility=hidden -O3 -Wall -Wno-maybe-uninitialized -march=rv64gc -DDRIVER=ADLIB $SPEEXDSP_COMPILE emu_x86.c -I$CURDIR/include -I../../include
c++ -c -fpic -fvisibility=hidden -O2 -Wall -march=rv64gc -fno-exceptions emu_opl2.cpp
cd dosbox_opl
c++ -c -fpic -fvisibility=hidden -O3 -Wall -march=rv64gc -fno-exceptions opl.cpp
cd ..
cd nuked_opl3
cc -c -fpic -fvisibility=hidden -O3 -Wall -march=rv64gc -DDISABLE_UNUSED_FUNCTIONS -DDISABLE_DIVISIONS opl3.c
cd ../..
cc -shared -Wl,-soname,adlib-dosbox_opl.so -o adlib-dosbox_opl.so -march=rv64gc *.o src/*.o src/dosbox_opl/*.o src/nuked_opl3/*.o -lm $SPEEXDSP_LINK -L../lib/riscv64

rm src/*.o
rm src/dosbox_opl/*.o
rm src/nuked_opl3/*.o

cd src
cc -c -fpic -fvisibility=hidden -O3 -Wall -Wno-maybe-uninitialized -march=rv64gc -DDRIVER=ROLAND $SPEEXDSP_COMPILE emu_x86.c -I$CURDIR/include -I../../include
c++ -c -fpic -fvisibility=hidden -O2 -Wall -march=rv64gc -fno-exceptions emu_mt32.cpp -I$CURDIR/src/munt-2.8
cd munt-2.8/mt32emu
c++ -c -fpic -fvisibility=hidden -O3 -Wall -march=rv64gc -fno-exceptions -DMT32EMU_WITH_STD_SNPRINTF *.cpp
rm FileStream.o* MidiStreamParser.o* SampleRateConverter.o* VersionTagging.o*
cd sha1
c++ -c -fpic -fvisibility=hidden -O3 -Wall -march=rv64gc -fno-exceptions sha1.cpp
cd ../../../..
cc -shared -Wl,-soname,mt32-munt.so -o mt32-munt.so -march=rv64gc *.o src/*.o src/munt-2.8/mt32emu/*.o src/munt-2.8/mt32emu/sha1/*.o -lm $SPEEXDSP_LINK -lstdc++ -L../lib/riscv64

rm src/*.o
rm src/munt-2.8/mt32emu/*.o
rm src/munt-2.8/mt32emu/sha1/*.o

cd src
cc -c -fpic -fvisibility=hidden -O3 -Wall -Wno-maybe-uninitialized -march=rv64gc -DDRIVER=EMU8000 $SPEEXDSP_COMPILE emu_x86.c -I$CURDIR/include -I../../include
cc -c -fpic -fvisibility=hidden -O2 -Wall -march=rv64gc emu_awe32.c
cd pcem_emu8k
cc -c -fpic -fvisibility=hidden -O3 -Wall -march=rv64gc sound_emu8k.c
cd ../..
cc -shared -Wl,-soname,awe32-emu8k.so -o awe32-emu8k.so -march=rv64gc *.o src/*.o src/pcem_emu8k/*.o -lm $SPEEXDSP_LINK -L../lib/riscv64

rm *.o
rm src/*.o
rm src/pcem_emu8k/*.o

cd ..
