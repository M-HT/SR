@echo off
gcc -c -m32 -O2 -Wall scaler-hqx.c
cd hqx
gcc -c -m32 -march=x86-64 -O3 -Wall *.c *.S
cd ..
gcc -s -shared -o scaler-hqx.dll -m32 scaler-hqx.o hqx/*.o

g++ -c -m32 -O2 -Wall -std=c++11 scaler-xbrz.cpp
cd xbrz
g++ -c -m32 -march=x86-64 -O3 -Wall -Wno-strict-aliasing -Wno-attributes -Wno-uninitialized -std=c++11 -fno-threadsafe-statics -DNO_BUFFER_HEAP_ALLOCATION -DNO_EXTRA_SCALERS xbrz.cpp
cd ..
gcc -s -shared -o scaler-xbrz.dll -m32 scaler-xbrz.o xbrz/*.o

rm *.o
rm hqx/*.o
rm xbrz/*.o
