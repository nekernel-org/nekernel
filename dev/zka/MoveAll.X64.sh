#! /bin/sh

for file in *.o; do
	mv -- "$file" "${file%.o}.obj"
done

mv *.obj HALKit/AMD64/*.obj obj/
