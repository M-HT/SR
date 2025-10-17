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
  SET SPEEXDSP_COMPILE=/DUSE_SPEEXDSP_RESAMPLER
  SET SPEEXDSP_LINK=speexdsp.lib
) ELSE (
  SET SPEEXDSP_COMPILE=
  SET SPEEXDSP_LINK=
)

cl /nologo /c /O2 /W3 /MD %SPEEXDSP_COMPILE% midi-wildmidi.c /I.\include /D_CRT_SECURE_NO_WARNINGS
link /NOLOGO /DLL /OUT:midi-wildmidi.dll midi-wildmidi.obj /DEF:midi.def /MACHINE:X86 libWildMidi.lib %SPEEXDSP_LINK% /LIBPATH:.\lib\x86
cl /nologo /c /O2 /W3 /MD midi-bassmidi.c /I.\include
link /NOLOGO /DLL /OUT:midi-bassmidi.dll midi-bassmidi.obj /DEF:midi.def /MACHINE:X86 bassmidi.lib bass.lib /LIBPATH:.\lib\x86
cl /nologo /c /O2 /W3 /MD %SPEEXDSP_COMPILE% midi-adlmidi.c /I.\include
link /NOLOGO /DLL /OUT:midi-adlmidi.dll midi-adlmidi.obj /DEF:midi.def /MACHINE:X86 ADLMIDI.lib %SPEEXDSP_LINK% /LIBPATH:.\lib\x86
cl /nologo /c /O2 /W3 /MD midi2-windows.c
link /NOLOGO /DLL /OUT:midi2-windows.dll midi2-windows.obj /DEF:midi2.def /MACHINE:X86 winmm.lib

del *.obj *.exp *.lib

cd xcom12

cl /nologo /c /O2 /W3 /MD mpsmusic-driver.c /I.\include /I.. /D_CRT_SECURE_NO_WARNINGS
cd src
cl /nologo /c /O2 /W3 /MD /DDRIVER=ADLIB %SPEEXDSP_COMPILE% emu_x86.c /I.\include /I..\..\include /D_CRT_SECURE_NO_WARNINGS
cl /nologo /c /O2 /W3 /MD emu_opl2.cpp
cd dosbox_opl
cl /nologo /c /O2 /W3 /MD opl.cpp
cd ..
cd nuked_opl3
cl /nologo /c /O2 /W3 /MD /arch:SSE2 /DDISABLE_UNUSED_FUNCTIONS opl3.c
cd ../..
link /NOLOGO /DLL /OUT:adlib-dosbox_opl.dll *.obj src/*.obj src/dosbox_opl/*.obj src/nuked_opl3/*.obj /DEF:..\midi.def /MACHINE:X86 %SPEEXDSP_LINK% /LIBPATH:..\lib\x86

del src\*.obj
del src\dosbox_opl\*.obj
del src\nuked_opl3\*.obj

cd src
cl /nologo /c /O2 /W3 /MD /DDRIVER=ROLAND %SPEEXDSP_COMPILE% emu_x86.c /I.\include /I..\..\include /D_CRT_SECURE_NO_WARNINGS
cl /nologo /c /O2 /W3 /MD /EHsc emu_mt32.cpp /I.\munt-2.7.0 /D_CRT_SECURE_NO_WARNINGS
cd munt-2.7.0/mt32emu
cl /nologo /c /O2 /W3 /MD /EHsc *.cpp /D_CRT_SECURE_NO_WARNINGS
cd sha1
cl /nologo /c /O2 /W3 /MD sha1.cpp
cd ../../../..
link /NOLOGO /DLL /OUT:mt32-munt.dll *.obj src/*.obj src/munt-2.7.0/mt32emu/*.obj src/munt-2.7.0/mt32emu/sha1/*.obj /DEF:..\midi.def /MACHINE:X86 %SPEEXDSP_LINK% /LIBPATH:..\lib\x86

del src\*.obj
del src\munt-2.7.0\mt32emu\*.obj
del src\munt-2.7.0\mt32emu\sha1\*.obj

cd src
cl /nologo /c /O2 /W3 /MD /DDRIVER=EMU8000 %SPEEXDSP_COMPILE% emu_x86.c /I.\include /I..\..\include /D_CRT_SECURE_NO_WARNINGS
cl /nologo /c /O2 /W3 /MD emu_awe32.c /D_CRT_SECURE_NO_WARNINGS
cd pcem_emu8k
cl /nologo /c /O2 /W3 /wd4244 /MD /arch:SSE2 sound_emu8k.c
cd ../..
link /NOLOGO /DLL /OUT:awe32-emu8k.dll *.obj src/*.obj src/pcem_emu8k/*.obj /DEF:..\midi.def /MACHINE:X86 %SPEEXDSP_LINK% /LIBPATH:..\lib\x86

del *.obj *.exp *.lib
del src\*.obj
del src\pcem_emu8k\*.obj

cd ..
