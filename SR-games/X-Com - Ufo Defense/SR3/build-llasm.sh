#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp llasm/*.sci ./
./SR.exe INTRO.EXE intro-asm.asm >a.a 2>b.a
rm *.sci
./compact_source_llasm.py
rm *.a
