#!/bin/sh
cc -c -fpic -fvisibility=hidden -m32 -O2 -Wall scaler-hqx.c
cd hqx
cc -c -fpic -fvisibility=hidden -m32 -march=x86-64 -O3 -Wall *.c *.S
cd ..
cc -shared -Wl,-soname,scaler-hqx.so -o scaler-hqx.so -fpic -m32 scaler-hqx.o hqx/*.o

# find maximum supported C++ standard in GCC
CPPSTD=
c++ -c -fpic -m32 -O2 -Wall -std=c++23 scaler-xbrz.cpp 2>/dev/null
if [ $? -eq 0 ]; then
    CPPSTD=c++23
else
    c++ -c -fpic -m32 -O2 -Wall -std=c++20 scaler-xbrz.cpp 2>/dev/null
    if [ $? -eq 0 ]; then
        CPPSTD=c++20
    else
        c++ -c -fpic -m32 -O2 -Wall -std=c++17 scaler-xbrz.cpp 2>/dev/null
        if [ $? -eq 0 ]; then
            CPPSTD=c++17
        else
            c++ -c -fpic -m32 -O2 -Wall -std=c++14 scaler-xbrz.cpp 2>/dev/null
            if [ $? -eq 0 ]; then
                CPPSTD=c++14
            else
                CPPSTD=c++11
            fi
        fi
    fi
fi

c++ -c -fpic -fvisibility=hidden -m32 -O2 -Wall -std=${CPPSTD} scaler-xbrz.cpp
cd xbrz
c++ -c -fpic -fvisibility=hidden -ffast-math -m32 -march=x86-64 -O3 -Wall -Wno-strict-aliasing -Wno-attributes -Wno-uninitialized -std=${CPPSTD} -fno-threadsafe-statics -DNO_BUFFER_HEAP_ALLOCATION -DNO_EXTRA_SCALERS -DNO_ALPHA_SUPPORT xbrz.cpp
cd ..
cc -shared -Wl,-soname,scaler-xbrz.so -o scaler-xbrz.so -fpic -m32 scaler-xbrz.o xbrz/*.o

rm *.o
rm hqx/*.o
rm xbrz/*.o
