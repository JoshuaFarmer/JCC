./bin/jcc test.c -o test.s
nasm -f elf32 -o program.o test.s
ld -m elf_i386 -s -o program program.o
