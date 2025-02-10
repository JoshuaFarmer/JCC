# JDECL Compilation
for file in tests/src/*.c; do
    base=$(basename "$file" .c)
    ./bin/jcc-i386 "$file" -o "tests/s/${base}_j.s"
    nasm "tests/s/${base}_j.s" -o "tests/o/${base}_j.o" -felf32
    ld -m elf_i386 -s -o "tests/exe/${base}_j" "tests/o/${base}_j.o"
    "./tests/exe/${base}_j"
    echo "JDECL RETURN CODE: $?"
done

# CDECL Compilation
for file in tests/src/*.c; do
    base=$(basename "$file" .c)
    ./bin/jcc-i386-cdecl "$file" -o "tests/s/${base}_c.s"
    nasm "tests/s/${base}_c.s" -o "tests/o/${base}_c.o" -felf32
    gcc "tests/o/${base}_c.o" -o "tests/exe/${base}_c" -m32
    "./tests/exe/${base}_c"
    echo "CDECL RETURN CODE: $?"
done
