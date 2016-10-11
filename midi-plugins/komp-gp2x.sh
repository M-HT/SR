#!/bin/sh
/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -shared -Wl,-soname,midi-wildmidi.so -o midi-wildmidi.so -fpic -O2 -Wall -march=armv4t -mtune=arm920t midi-wildmidi.c -I`pwd`/include -lWildMidi -L`pwd`/lib/armv4

