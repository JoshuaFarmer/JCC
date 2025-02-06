./bin/jcc test.c -o test.s
nasm -f elf32 -o program.o test.s
gcc program.o -o program -m32