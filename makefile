all: main mkfs
	rm -f root

main: main.o myfs.o mylibc.o
	gcc -o $@ $^

mkfs: mkfs.o myfs.o
	gcc -o $@ $^

%.o: %.c
	gcc -c -g $<

clean:
	rm -rf *.o main root mkfs