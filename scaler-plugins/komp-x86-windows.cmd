@echo off
set CC=gcc
set CXX=g++

%CC% -c -m32 -O2 -Wall scaler-hqx.c
cd hqx
%CC% -c -m32 -march=x86-64 -O3 -Wall *.c *.S
cd ..
%CC% -s -shared -o scaler-hqx.dll -m32 scaler-hqx.o hqx/*.o scaler.def

rem find maximum supported C++ standard in GCC
SET CPPSTD=c++23
%CXX% -c -m64 -O2 -Wall -std=%CPPSTD% scaler-xbrz.cpp 2>NUL
IF NOT ERRORLEVEL 1 GOTO :stdok

SET CPPSTD=c++20
%CXX% -c -m64 -O2 -Wall -std=%CPPSTD% scaler-xbrz.cpp 2>NUL
IF NOT ERRORLEVEL 1 GOTO :stdok

SET CPPSTD=c++17
%CXX% -c -m64 -O2 -Wall -std=%CPPSTD% scaler-xbrz.cpp 2>NUL
IF NOT ERRORLEVEL 1 GOTO :stdok

SET CPPSTD=c++14
%CXX% -c -m64 -O2 -Wall -std=%CPPSTD% scaler-xbrz.cpp 2>NUL
IF NOT ERRORLEVEL 1 GOTO :stdok

SET CPPSTD=c++11
:stdok

%CXX% -c -m32 -O2 -Wall -std=%CPPSTD% scaler-xbrz.cpp
cd xbrz
%CXX% -c -ffast-math -m32 -march=x86-64 -O3 -Wall -Wno-strict-aliasing -Wno-attributes -Wno-uninitialized -std=%CPPSTD% -fno-threadsafe-statics -DNO_BUFFER_HEAP_ALLOCATION -DNO_EXTRA_SCALERS -DNO_ALPHA_SUPPORT xbrz.cpp
cd ..
%CC% -s -shared -o scaler-xbrz.dll -m32 scaler-xbrz.o xbrz/*.o scaler.def

del *.o
del hqx\*.o
del xbrz\*.o
