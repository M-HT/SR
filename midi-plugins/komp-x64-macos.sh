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

cc -bundle -o midi-wildmidi.so -fpic -fvisibility=hidden -arch x86_64 -O2 -Wall $SPEEXDSP_COMPILE midi-wildmidi.c -I`pwd`/include -lWildMidi $SPEEXDSP_LINK -L`pwd`/lib/x64
cc -bundle -o midi-bassmidi.so -fpic -fvisibility=hidden -arch x86_64 -O2 -Wall midi-bassmidi.c -I`pwd`/include -lbassmidi -lbass -L`pwd`/lib/x64
cc -bundle -o midi-adlmidi.so -fpic -fvisibility=hidden -arch x86_64 -O2 -Wall $SPEEXDSP_COMPILE midi-adlmidi.c -I`pwd`/include -lADLMIDI $SPEEXDSP_LINK -lstdc++ -L`pwd`/lib/x64
cc -bundle -o midi2-coremidi.so -fpic -fvisibility=hidden -arch x86_64 -O2 -Wall midi2-coremidi.c -lpthread -framework CoreMIDI -framework CoreFoundation

cd xcom12

CURDIR=`pwd`
cc -c -fpic -fvisibility=hidden -arch x86_64 -O2 -Wall mpsmusic-driver.c -I$CURDIR/include -I../
cd src
cc -c -fpic -fvisibility=hidden -arch x86_64 -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=ADLIB $SPEEXDSP_COMPILE emu_x86.c -I$CURDIR/include -I../../include
c++ -c -fpic -fvisibility=hidden -arch x86_64 -O2 -Wall -fno-exceptions emu_opl2.cpp
cd dosbox_opl
c++ -c -fpic -fvisibility=hidden -arch x86_64 -O3 -Wall -fno-exceptions opl.cpp
cd ..
cd nuked_opl3
cc -c -fpic -fvisibility=hidden -arch x86_64 -O3 -Wall -DDISABLE_UNUSED_FUNCTIONS opl3.c
cd ../..
cc -bundle -o adlib-dosbox_opl.so -arch x86_64 *.o src/*.o src/dosbox_opl/*.o src/nuked_opl3/*.o -lm $SPEEXDSP_LINK -L../lib/x64

rm src/*.o
rm src/dosbox_opl/*.o
rm src/nuked_opl3/*.o

cd src
cc -c -fpic -fvisibility=hidden -arch x86_64 -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=ROLAND $SPEEXDSP_COMPILE emu_x86.c -I$CURDIR/include -I../../include
c++ -c -fpic -fvisibility=hidden -arch x86_64 -O2 -Wall -fno-exceptions emu_mt32.cpp -I$CURDIR/src/munt-2.7.0
cd munt-2.7.0/mt32emu
c++ -c -fpic -fvisibility=hidden -arch x86_64 -O3 -Wall -fno-exceptions *.cpp
rm FileStream.o* MidiStreamParser.o* SampleRateConverter.o* VersionTagging.o*
cd sha1
c++ -c -fpic -fvisibility=hidden -arch x86_64 -O3 -Wall -fno-exceptions sha1.cpp
cd ../../../..
cc -bundle -o mt32-munt.so -arch x86_64 *.o src/*.o src/munt-2.7.0/mt32emu/*.o src/munt-2.7.0/mt32emu/sha1/*.o -lm $SPEEXDSP_LINK -lstdc++ -L../lib/x64

rm src/*.o
rm src/munt-2.7.0/mt32emu/*.o
rm src/munt-2.7.0/mt32emu/sha1/*.o

cd src
cc -c -fpic -fvisibility=hidden -arch x86_64 -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=EMU8000 $SPEEXDSP_COMPILE emu_x86.c -I$CURDIR/include -I../../include
cc -c -fpic -fvisibility=hidden -arch x86_64 -O2 -Wall emu_awe32.c
cd pcem_emu8k
cc -c -fpic -fvisibility=hidden -arch x86_64 -O3 -Wall sound_emu8k.c
cd ../..
cc -bundle -o awe32-emu8k.so -arch x86_64 *.o src/*.o src/pcem_emu8k/*.o -lm $SPEEXDSP_LINK -L../lib/x64

rm *.o
rm src/*.o
rm src/pcem_emu8k/*.o

cd ..
