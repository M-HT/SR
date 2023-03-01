#!/bin/sh
$PNDSDK/bin/pandora-gcc -shared -Wl,-soname,midi-wildmidi.so -o midi-wildmidi.so -fpic -fvisibility=hidden -O2 -Wall midi-wildmidi.c -I`pwd`/include -lWildMidi -L`pwd`/lib/arm
$PNDSDK/bin/pandora-gcc -shared -Wl,-soname,midi-bassmidi.so -o midi-bassmidi.so -fpic -fvisibility=hidden -O2 -Wall midi-bassmidi.c -I`pwd`/include -lbassmidi -lbass -L`pwd`/lib/arm
$PNDSDK/bin/pandora-gcc -shared -Wl,-soname,midi-adlmidi.so -o midi-adlmidi.so -fpic -fvisibility=hidden -O2 -Wall midi-adlmidi.c -I`pwd`/include -lADLMIDI -lstdc++ -L`pwd`/lib/arm
$PNDSDK/bin/pandora-gcc -shared -Wl,-soname,midi2-alsa.so -o midi2-alsa.so -fpic -fvisibility=hidden -O2 -Wall midi2-alsa.c -I$PNDSDK/usr/include -lasound -lpthread -L$PNDSDK/usr/lib

cd albion

$PNDSDK/bin/pandora-gcc -shared -Wl,-soname,midiA-wildmidi.so -o midiA-wildmidi.so -fpic -fvisibility=hidden -O2 -Wall midiA-wildmidi.c -I`pwd`/.. -I`pwd`/include -lWildMidiA -L`pwd`/lib/arm

cd ..

cd xcom12

CURDIR=`pwd`
$PNDSDK/bin/pandora-gcc -c -fpic -fvisibility=hidden -O2 -Wall mpsmusic-driver.c -I$CURDIR/include -I../
cd src
$PNDSDK/bin/pandora-gcc -c -fpic -fvisibility=hidden -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=ADLIB emu_x86.c -I$CURDIR/include
$PNDSDK/bin/pandora-g++ -c -fpic -fvisibility=hidden -O2 -Wall -fno-exceptions emu_opl2.cpp
cd dosbox_opl
$PNDSDK/bin/pandora-g++ -c -fpic -fvisibility=hidden -O3 -Wall -fno-exceptions opl.cpp
cd ../..
$PNDSDK/bin/pandora-gcc -shared -Wl,-soname,adlib-dosbox_opl.so -o adlib-dosbox_opl.so *.o src/*.o src/dosbox_opl/*.o

rm src/*.o
rm src/dosbox_opl/*.o

#cd src
#$PNDSDK/bin/pandora-gcc -c -fpic -fvisibility=hidden -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=ROLAND emu_x86.c -I$CURDIR/include
#$PNDSDK/bin/pandora-g++ -c -fpic -fvisibility=hidden -O2 -Wall -fno-exceptions emu_mt32.cpp -I$CURDIR/src/munt-1.4.0
#cd munt-1.4.0/mt32emu
#$PNDSDK/bin/pandora-g++ -c -fpic -fvisibility=hidden -O3 -Wall -fno-exceptions *.cpp
#cd sha1
#$PNDSDK/bin/pandora-g++ -c -fpic -fvisibility=hidden -O3 -Wall -fno-exceptions sha1.cpp
#cd ../../../..
#$PNDSDK/bin/pandora-gcc -shared -Wl,-soname,mt32-munt.so -o mt32-munt.so *.o src/*.o src/munt-1.4.0/mt32emu/*.o src/munt-1.4.0/mt32emu/sha1/*.o -lstdc++

rm *.o
#rm src/*.o
#rm src/munt-1.4.0/mt32emu/*.o
#rm src/munt-1.4.0/mt32emu/sha1/*.o

cd ..
