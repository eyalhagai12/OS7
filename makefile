all: main
	touch disk

main: main.o myfs.o
	gcc -o $@ $^

%.o: %.c
	gcc -c -g $<

clean:
	rm -rf *.o main disk