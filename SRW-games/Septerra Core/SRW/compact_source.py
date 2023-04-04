#! /usr/bin/python3
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
                elif cRepStr.startswith("resb"):
                    fOut.write("resb " + str(iNum) + "\n")
                elif iNum == 2:
                    fOut.write(cRepStr)
                    fOut.write(cRepStr)
                else:
                    fOut.write("times " + str(iNum) + " " + cRepStr)
                iNum = 0
                #cRepStr = ""

        if iNum == 0:
            if cLine.startswith("db "):
                iNum = 1
                cRepStr = cLine
            elif cLine.strip() == "resb 1":
                iNum = 1
                cRepStr = cLine
            else:
                fOut.write(cLine)

    if iNum != 0:
        if iNum == 1:
            fOut.write(cRepStr)
        elif cRepStr.startswith("resb"):
            fOut.write("resb " + str(iNum) + "\n")
        elif iNum == 2:
            fOut.write(cRepStr)
            fOut.write(cRepStr)
        else:
            fOut.write("times " + str(iNum) + " " + cRepStr)

    fOut.close()
    fIn.close()

    os.remove(cFile)
    os.rename(cTmpFile, cFile)

Compact(cPath + "seg01.inc")
Compact(cPath + "seg02.inc")
Compact(cPath + "seg03.inc")
Compact(cPath + "seg05.inc")
