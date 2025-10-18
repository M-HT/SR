@echo off
cl /nologo /c /O2 /W3 /MD scaler-hqx.c
cd hqx
cl /nologo /c /O2 /W3 /MD /arch:SSE2 *.c
cd ..
link /NOLOGO /DLL /OUT:scaler-hqx.dll scaler-hqx.obj hqx/*.obj /DEF:scaler.def /MACHINE:X64

rem find maximum supported C++ standard in MSVC
SET CPPSTD=/std:c++20
cl /? 2>NUL | findstr /C:"c++20" >NUL
IF NOT ERRORLEVEL 1 GOTO :stdok

SET CPPSTD=/std:c++17
cl /? 2>NUL | findstr /C:"c++17" >NUL
IF NOT ERRORLEVEL 1 GOTO :stdok

SET CPPSTD=/std:c++14
cl /? 2>NUL | findstr /C:"c++14" >NUL
IF NOT ERRORLEVEL 1 GOTO :stdok

SET CPPSTD=
:stdok

cl /nologo /c /O2 /W3 /wd4305 /MD %CPPSTD% /EHsc scaler-xbrz.cpp
cd xbrz
cl /nologo /c /O2 /W3 /wd4305 /wd4244 /MD %CPPSTD% /arch:SSE2 /fp:fast /DNO_BUFFER_HEAP_ALLOCATION /DNO_EXTRA_SCALERS /DNO_ALPHA_SUPPORT xbrz.cpp
cd ..
link /NOLOGO /DLL /OUT:scaler-xbrz.dll scaler-xbrz.obj xbrz/*.obj /DEF:scaler.def /MACHINE:X64


del *.obj *.exp *.lib
del hqx\*.obj
del xbrz\*.obj
