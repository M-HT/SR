#!/bin/sh
gcc -s -O2 -Wall -fPIE -pie -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-builtin -fno-stack-protector -Wl,-z,noexecstack SR64-loader.c start-arm64.S syscall-arm64.c -o SR64-loader -nostdlib
