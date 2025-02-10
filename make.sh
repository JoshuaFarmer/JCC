#!/bin/bash
gcc -g "src/main.c" -o "bin/jcc-i386-cdecl" -DARCH_I386_CDECL
gcc -g "src/main.c" -o "bin/jcc-i386" -DARCH_I386_JDECL
mkdir -p tests/o tests/s tests/src tests/exe

sh test.sh