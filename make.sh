gcc "src/main.c" -o "bin/jcc-i386-cdecl" -DARCH_I386_CDECL
gcc "src/main.c" -o "bin/jcc-i386" -DARCH_I386_JDECL
sh lines.sh
sh test.sh