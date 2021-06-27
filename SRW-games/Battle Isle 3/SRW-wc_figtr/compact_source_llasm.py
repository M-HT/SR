#! /usr/bin/python
cPath = "./"

import os

def Compact (cFile):
    cTmpFile = cFile + "tmp"
    fIn = open(cFile, "rt")
    fOut = open(cTmpFile, "wt")

    iNum = 0
    cRepStr = ""

    for cLine in fIn:
        if iNum != 0:
            if cLine == cRepStr:
                iNum = iNum + 1
            else:
                if iNum == 1:
                    fOut.write(cRepStr)
                elif cRepStr.startswith("dskip"):
                    fOut.write("dskip " + str(iNum) + "\n")
                elif iNum == 2:
                    fOut.write(cRepStr)
                    fOut.write(cRepStr)
                else:
                    fOut.write(cRepStr.rstrip() + " dup " + str(iNum) + cRepStr[len(cRepStr.rstrip()):])
                iNum = 0
                #cRepStr = ""

        if iNum == 0:
            if cLine.startswith("db "):
                iNum = 1
                cRepStr = cLine
            elif cLine.strip() == "dskip 1":
                iNum = 1
                cRepStr = cLine
            elif iNum == 2:
                fOut.write(cRepStr)
                fOut.write(cRepStr)
            else:
                fOut.write(cLine)

    if iNum != 0:
        if iNum == 1:
            fOut.write(cRepStr)
        elif cRepStr.startswith("dskip"):
            fOut.write("dskip " + str(iNum) + "\n")
        else:
            fOut.write(cRepStr.rstrip() + " dup " + str(iNum) + cRepStr[len(cRepStr.rstrip()):])

    fOut.close()
    fIn.close()

    os.remove(cFile)
    os.rename(cTmpFile, cFile)

Compact(cPath + "seg01_data.llinc")
Compact(cPath + "seg08_data.llinc")
Compact(cPath + "seg09_data.llinc")
Compact(cPath + "seg10_data.llinc")
Compact(cPath + "seg11_data.llinc")
Compact(cPath + "seg12_data.llinc")
Compact(cPath + "seg13_data.llinc")
