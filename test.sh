echo "i386-jdecl"

# JDECL Compilation
for file in tests/src/*.c; do
    base=$(basename "$file" .c)
    if [ $base -eq "5" ]; then #uses printf, not available with this format
        continue
    fi
    ./bin/jcc-i386 "$file" -o "tests/s/${base}_j.s"
    ./bin/jcc-i8086 "$file" -o "tests/s/${base}_8086.s"
    nasm "tests/s/${base}_j.s" -o "tests/o/${base}_j.o" -felf32
    ld -m elf_i386 -s -o "tests/exe/${base}_j" "tests/o/${base}_j.o"
    "./tests/exe/${base}_j"
    echo "$file (JDECL) RETURN CODE: $?"
done

echo "i386-cdecl"

# CDECL Compilation
for file in tests/src/*.c; do
    base=$(basename "$file" .c)
    ./bin/jcc-i386-cdecl "$file" -o "tests/s/${base}_c.s"
    nasm "tests/s/${base}_c.s" -o "tests/o/${base}_c.o" -felf32
    gcc "tests/o/${base}_c.o" -o "tests/exe/${base}_c" -m32
    "./tests/exe/${base}_c"
    echo "$file (CDECL) RETURN CODE: $?"
done

echo "cisc"

# custom cpu compilation
for file in tests/src/*.c; do
    base=$(basename "$file" .c)
    ./bin/jcc-cisc "$file" -o "tests/s/${base}_cisc.s"
    echo "compiled $file"
done

