#! /bin/sh
cd "`echo $0 | sed 's/\/[^\/]*$//'`"
cp llasm/*.sci ./
./SRW.exe WC_FIGTR.DLL WC_FIGTR.llasm >a.a 2>b.a
rm *.sci
./compact_source_llasm.py
rm *.a
