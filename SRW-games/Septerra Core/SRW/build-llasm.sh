#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp llasm/*.sci ./
./SRW.exe Septerra104.exe Septerra.llasm >a.a 2>b.a
rm *.sci
./compact_source_llasm.py
rm *.a
rm Septerra.resdump
