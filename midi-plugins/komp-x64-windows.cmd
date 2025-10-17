@echo off

rem to enable using SpeexDSP either run the script with argument USE_SPEEXDSP or define non-empty environment variable USE_SPEEXDSP
SET LOCAL_USE_SPEEXDSP=

:loop
IF NOT "%1" == "" (
  IF "%1"=="USE_SPEEXDSP" (
    SET LOCAL_USE_SPEEXDSP=1
  ) else (
      echo Unknown argument: %1
  )
  SHIFT
  GOTO :loop
)

IF NOT "%USE_SPEEXDSP%" == "" SET LOCAL_USE_SPEEXDSP=1

IF NOT "%LOCAL_USE_SPEEXDSP%" == "" (
  SET SPEEXDSP_COMPILE=-DUSE_SPEEXDSP_RESAMPLER
  SET SPEEXDSP_LINK=-lspeexdsp
) ELSE (
  SET SPEEXDSP_COMPILE=
  SET SPEEXDSP_LINK=
)

gcc -s -shared -static-libgcc -o midi-wildmidi.dll -m64 -O2 -Wall %SPEEXDSP_COMPILE% midi-wildmidi.c midi.def -I./include -lWildMidi %SPEEXDSP_LINK% -L./lib/x64
gcc -s -shared -o midi-bassmidi.dll -m64 -O2 -Wall midi-bassmidi.c midi.def -I./include -lbassmidi -lbass -L./lib/x64
gcc -s -shared -static-libgcc -o midi-adlmidi.dll -m64 -O2 -Wall %SPEEXDSP_COMPILE% midi-adlmidi.c midi.def -I./include -lADLMIDI %SPEEXDSP_LINK% -L./lib/x64
gcc -s -shared -static-libgcc -o midi2-windows.dll -m64 -O2 -Wall midi2-windows.c midi2.def -lwinmm

cd xcom12

gcc -c -m64 -O2 -Wall mpsmusic-driver.c -I./include -I../
cd src
gcc -c -m64 -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=ADLIB %SPEEXDSP_COMPILE% emu_x86.c -I./include -I../../include
g++ -c -m64 -O2 -Wall -fno-exceptions emu_opl2.cpp
cd dosbox_opl
g++ -c -m64 -O3 -Wall -fno-exceptions opl.cpp
cd ..
cd nuked_opl3
gcc -c -m64 -march=x86-64 -O3 -Wall -DDISABLE_UNUSED_FUNCTIONS opl3.c
cd ../..
gcc -s -shared -static-libgcc -o adlib-dosbox_opl.dll -m64 *.o src/*.o src/dosbox_opl/*.o src/nuked_opl3/*.o ../midi.def -lm %SPEEXDSP_LINK% -L../lib/x64

del src\*.o
del src\dosbox_opl\*.o
del src\nuked_opl3\*.o

cd src
gcc -c -m64 -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=ROLAND %SPEEXDSP_COMPILE% emu_x86.c -I./include -I../../include
g++ -c -m64 -O2 -Wall -fno-exceptions emu_mt32.cpp -I./munt-2.7.0
cd munt-2.7.0/mt32emu
g++ -c -m64 -O3 -Wall -fno-exceptions *.cpp
cd sha1
g++ -c -m64 -O3 -Wall -fno-exceptions sha1.cpp
cd ../../../..
gcc -s -shared -static-libgcc -o mt32-munt.dll -m64 *.o src/*.o src/munt-2.7.0/mt32emu/*.o src/munt-2.7.0/mt32emu/sha1/*.o ../midi.def -lstdc++ -lm %SPEEXDSP_LINK% -L../lib/x64

del src\*.o
del src\munt-2.7.0\mt32emu\*.o
del src\munt-2.7.0\mt32emu\sha1\*.o

cd src
gcc -c -m64 -O3 -Wall -Wno-maybe-uninitialized -DDRIVER=EMU8000 %SPEEXDSP_COMPILE% emu_x86.c -I./include -I../../include
gcc -c -m64 -O2 -Wall -fno-exceptions emu_awe32.c
cd pcem_emu8k
gcc -c -m64 -march=x86-64 -O3 -Wall sound_emu8k.c
cd ../..
gcc -s -shared -static-libgcc -o awe32-emu8k.dll -m64 *.o src/*.o src/pcem_emu8k/*.o ../midi.def -lm %SPEEXDSP_LINK% -L../lib/x64

del *.o
del src\*.o
del src\pcem_emu8k\*.o

cd ..
