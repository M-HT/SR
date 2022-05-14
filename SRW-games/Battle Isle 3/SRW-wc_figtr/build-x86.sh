#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp x86/*.sci ./
./SRW.exe WC_FIGTR.DLL WC_FIGTR.asm >a.a 2>b.a
rm *.sci
./compact_source.py
rm *.a
rm WC_FIGTR.def
