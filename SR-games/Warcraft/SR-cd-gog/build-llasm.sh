#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp llasm/*.sci ./
./SR.exe WAR.EXE war-asm-cdgog.asm >a.a 2>b.a
rm *.sci
./compact_source_llasm.py
rm *.a
