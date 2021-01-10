#!/bin/sh
gcc -c -fpic -m32 -O2 -Wall scaler-hqx.c
cd hqx
gcc -c -fpic -m32 -march=x86-64 -O3 -Wall *.c *.S
cd ..
gcc -shared -Wl,-soname,scaler-hqx.so -o scaler-hqx.so -fpic -m32 scaler-hqx.o hqx/*.o

# find maximum supported C++ standard in GCC
CPPSTD=
g++ -c -fpic -m32 -O2 -Wall -std=c++20 scaler-xbrz.cpp 2>/dev/null
if [ $? -eq 0 ]; then
    CPPSTD=c++20
else
    g++ -c -fpic -m32 -O2 -Wall -std=c++17 scaler-xbrz.cpp 2>/dev/null
    if [ $? -eq 0 ]; then
        CPPSTD=c++17
    else
        g++ -c -fpic -m32 -O2 -Wall -std=c++14 scaler-xbrz.cpp 2>/dev/null
        if [ $? -eq 0 ]; then
            CPPSTD=c++14
        else
            CPPSTD=c++11
        fi
    fi
fi

g++ -c -fpic -m32 -O2 -Wall -std=${CPPSTD} scaler-xbrz.cpp
cd xbrz
g++ -c -fpic -m32 -march=x86-64 -O3 -Wall -Wno-strict-aliasing -Wno-attributes -Wno-uninitialized -std=${CPPSTD} xbrz.cpp
cd ..
gcc -shared -Wl,-soname,scaler-xbrz.so -o scaler-xbrz.so -fpic -m32 scaler-xbrz.o xbrz/*.o -lstdc++

rm *.o
rm hqx/*.o
rm xbrz/*.o
