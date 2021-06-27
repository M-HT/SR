#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp x86/*.sci ./
./SRW.exe SDI_1R.EXE SDI_1R.asm >a.a 2>b.a
rm *.sci
./compact_source.py
rm *.a
rm SDI_1R.resdump SDI_1R.def
