gcc "src/main.c" -o "bin/jcc-i386-cdecl" -DARCH_I386 -DCALL_CDECL
gcc "src/main.c" -o "bin/jcc-i386" -DARCH_I386 -DCALL_JDECL
gcc "src/main.c" -o "bin/jcc-i85" -DARCH_I8085
sh lines.sh
sh test.sh