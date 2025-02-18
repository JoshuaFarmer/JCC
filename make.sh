gcc "src/main.c" -o "bin/jcc-i386-cdecl" -DARCH_I386 -DCALL_CDECL
gcc "src/main.c" -o "bin/jcc-i386" -DARCH_I386 -DCALL_JDECL
gcc "src/main.c" -o "bin/jcc-i85" -DARCH_I8085
gcc "src/main.c" -o "bin/jcc-i8086" -DARCH_I8086
gcc "src/main.c" -o "bin/jcc-cisc" -DARCH_CISC
sh lines.sh
#sh test.sh