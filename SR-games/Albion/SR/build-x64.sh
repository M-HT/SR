#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp x64/*.sci ./
./SR.exe MAIN.EXE Albion-main.asm >a.a 2>b.a
rm *.sci
./compact_source.py
nasm -felf64 -O1 -w+orphan-labels -w-number-overflow -ix64/ Albion-main.asm 2>a.a
./repair_short_jumps.py
nasm -felf64 -O1 -w+orphan-labels -w-number-overflow -ix64/ Albion-main.asm 2>a.a
./repair_short_jumps.py
rm *.a
