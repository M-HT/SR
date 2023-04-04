#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp arm/*.sci ./
./SR.exe TACTICAL.EXE tactical-asm-gog.asm >a.a 2>b.a
rm *.sci
./compact_source_arm.py
rm *.a
