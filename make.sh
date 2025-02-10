gcc -g "src/main.c" -o "bin/jcc-i386-cdecl" -DARCH_I386_CDECL
gcc -g "src/main.c" -o "bin/jcc-i386" -DARCH_I386_JDECL

for file in tests/*.c; do
        base="${file%.c}"

        ./bin/jcc-i386-cdecl "$file" -o "${base}_c.s"
        nasm "${base}_c.s" -o "${base}_c.o" -felf32
        gcc "${base}_c.o" -o "${base}_c" -m32
        "./${base}_c"
        echo "$file CDECL RETURN CODE: $?"
        
        ./bin/jcc-i386 "$file" -o "${base}_j.s"
        nasm "${base}_j.s" -o "${base}_j.o" -felf32
        ld -m elf_i386 -s -o "${base}_j" "${base}_j.o"
        "./${base}_j"
        echo "$file JDECL RETURN CODE: $?"
done
