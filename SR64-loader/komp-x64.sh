#!/bin/sh
gcc -s -m64 -O2 -Wall -fPIE -pie -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-builtin -fno-stack-protector -Wl,-z,noexecstack SR64-loader.c start-x64.S syscall-x64.c -o SR64-loader -nostdlib
