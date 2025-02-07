gcc -g -c "src/main.c" -o "bin/main.o"
gcc -g -c "src/expr.c" -o "bin/expr.o"
gcc -g "bin/main.o" "bin/expr.o" -o "bin/jcc"
./bin/jcc test.c -o test.s
if [ $? -eq 0 ]; then
        nasm test.s -o test.o -felf32
        gcc test.o -o test -m32
        ./test 2
        echo $?
fi
