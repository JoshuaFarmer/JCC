gcc "src/main.c" -o "bin/main.o" -O3 -c
gcc "src/expr.c" -o "bin/expr.o" -O3 -c
gcc "bin/main.o" "bin/expr.o" -o "bin/jcc"
./bin/jcc test.c -o test.s
nasm test.s -o test.o -felf32
gcc test.o -o test -m32