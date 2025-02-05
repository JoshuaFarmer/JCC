gcc "src/main.c" -o "bin/main.o" -O3 -c
gcc "src/expr.c" -o "bin/expr.o" -O3 -c
gcc "bin/main.o" "bin/expr.o" -o "bin/jcc"