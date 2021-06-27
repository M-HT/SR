#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp llasm/*.sci ./
./SRW.exe SDI_1R.EXE SDI_1R.llasm >a.a 2>b.a
rm *.sci
./compact_source_llasm.py
rm *.a
rm SDI_1R.resdump
