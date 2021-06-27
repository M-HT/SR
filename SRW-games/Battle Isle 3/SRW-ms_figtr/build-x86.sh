#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp x86/*.sci ./
./SRW.exe MS_FIGTR.DLL MS_FIGTR.asm >a.a 2>b.a
rm *.sci
./compact_source.py
rm *.a
rm MS_FIGTR.def
