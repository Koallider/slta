#!/bin/bash
nasm -f elf -l $1.lst $1.asm
gcc -m32 -o $1 $1.o