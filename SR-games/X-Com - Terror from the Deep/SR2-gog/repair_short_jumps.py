#! /usr/bin/python3
cPath = "./"
cErrFile = "a.a"

import os

fErr = open(cPath + cErrFile, "rt")

iErr = 0
cInFile = ""
cFile = ""
iLine = 0

for cErrLine in fErr:
    if cErrLine != "":
        iTemp1 = cErrLine.find(":")
        iTemp2 = cErrLine.find(":", iTemp1 + 1)
        cFile = cErrLine[ : iTemp1]
        iErr = int( cErrLine[iTemp1 + 1 : iTemp2] )

        if cErrLine[iTemp2 + 1 : ].startswith(" error: short jump is out of range"):
            if cFile != cInFile:
                if cInFile != "":
                    for cLine in fIn:
                        fOut.write(cLine)
                    fOut.close()
                    fIn.close()

                    os.remove(cPath + cInFile)
                    os.rename(cPath + cInFile + "tmp", cPath + cInFile)

                fIn = open(cPath + cFile, "rt")
                fOut = open(cPath + cFile + "tmp", "wt")
                cInFile = cFile
                iLine = 0

            while iLine + 1 < iErr:
                fOut.write(fIn.readline())
                iLine = iLine + 1

            cLine = fIn.readline()
            iLine = iLine + 1

            if cLine.startswith("jmp short "):
                # if line begins "jmp short" then remove "short" keyword
                cLine = "jmp" + cLine[9:]
            else:
                iTemp = cLine.find(" ")
                cFirst = cLine[ : iTemp + 1]
                cSecond = cLine[iTemp + 1 : ].lstrip()

                if cFirst.startswith("j") and cSecond.startswith("short "):
                    # if line begins "j?? short" then replace "short" keyword with "near" keyword
                    cLine = cFirst + "near " + cSecond[6:]
                else:
                    # else insert "near" keyword after first word
                    cLine = cFirst + "near " + cSecond

            fOut.write(cLine)

if cInFile != "":
    for cLine in fIn:
        fOut.write(cLine)
    fOut.close()
    fIn.close()

    os.remove(cPath + cInFile)
    os.rename(cPath + cInFile + "tmp", cPath + cInFile)

fErr.close()

