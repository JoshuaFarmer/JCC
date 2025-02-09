gcc -g -c "src/x86/main.c" -o "bin/main_x86.o"
gcc -g -c "src/x86/expr.c" -o "bin/expr_x86.o"
gcc -g "bin/main_x86.o" "bin/expr_x86.o" -o "bin/jcc-i386"

./bin/jcc-i386 test.c -o test.s
if [ $? -eq 0 ]; then
        nasm test.s -o test.o -felf32
        gcc test.o -o test -m32
        ./test
fi
