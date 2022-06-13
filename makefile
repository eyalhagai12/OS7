all: main

main: main.o myfs.o
	gcc -o $@ $^

%.o: %.c
	gcc -c -g $<

clean:
	rm -rf *.o main new_disk