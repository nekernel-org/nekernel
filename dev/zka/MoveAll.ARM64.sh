#! /bin/sh

for file in *.o; do
	mv -- "$file" "${file%.o}.obj"
done

mv *.obj obj/
