gcc -g "src/main.c" -o "bin/jcc-i386-cdecl" -DARCH_I386_CDECL
gcc -g "src/main.c" -o "bin/jcc-i386" -DARCH_I386_JDECL

./bin/jcc-i386-cdecl tests/1.c -o tests/1_c.s
if [ $? -eq 0 ]; then
        nasm tests/1_c.s -o tests/1_c.o -felf32
        gcc tests/1_c.o -o tests/1_c -m32
        ./tests/1_c
        echo 'CDECL RETURN CODE:' $?
fi

./bin/jcc-i386 tests/1.c -o tests/1_j.s
if [ $? -eq 0 ]; then
        nasm tests/1_j.s -o tests/1_j.o -felf32
        ld -m elf_i386 -s -o tests/1_j tests/1_j.o
        ./tests/1_j
        echo 'JDECL RETURN CODE:' $?
fi
