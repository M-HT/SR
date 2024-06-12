@echo off
gcc -s -shared -o midi-wildmidi.dll -m32 -O2 -Wall midi-wildmidi.c midi.def -I./include -lWildMidi -L./lib/x86
gcc -s -shared -o midi-bassmidi.dll -m32 -O2 -Wall midi-bassmidi.c midi.def -I./include -lbassmidi -lbass -L./lib/x86
gcc -s -shared -o midi-adlmidi.dll -m32 -O2 -Wall midi-adlmidi.c midi.def -I./include -lADLMIDI -L./lib/x86
gcc -s -shared -o midi2-windows.dll -m32 -O2 -Wall midi2-windows.c midi2.def -lwinmm

cd xcom12

gcc -c -m32 -O2 -Wall mpsmusic-driver.c -I./include -I../
cd src
gcc -c -m32 -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=ADLIB emu_x86.c -I./include
g++ -c -m32 -O2 -Wall -fno-exceptions emu_opl2.cpp
cd dosbox_opl
g++ -c -m32 -O3 -Wall -fno-exceptions opl.cpp
cd ..
cd nuked_opl3
gcc -c -m32 -march=x86-64 -O3 -Wall -DDISABLE_UNUSED_FUNCTIONS opl3.c
cd ../..
gcc -s -shared -o adlib-dosbox_opl.dll -m32 *.o src/*.o src/dosbox_opl/*.o src/nuked_opl3/*.o ../midi.def -lm

del src\*.o
del src\dosbox_opl\*.o
del src\nuked_opl3\*.o

cd src
gcc -c -m32 -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=ROLAND emu_x86.c -I./include
g++ -c -m32 -O2 -Wall -fno-exceptions emu_mt32.cpp -I./munt-2.7.0
cd munt-2.7.0/mt32emu
g++ -c -m32 -O3 -Wall -fno-exceptions *.cpp
cd sha1
g++ -c -m32 -O3 -Wall -fno-exceptions sha1.cpp
cd ../../../..
gcc -s -shared -o mt32-munt.dll -m32 *.o src/*.o src/munt-2.7.0/mt32emu/*.o src/munt-2.7.0/mt32emu/sha1/*.o ../midi.def -lstdc++ -lm

del *.o
del src\*.o
del src\munt-2.7.0\mt32emu\*.o
del src\munt-2.7.0\mt32emu\sha1\*.o

cd ..
