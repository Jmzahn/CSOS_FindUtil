all: find

prog: find.c
	gcc find.c -o find

clean:
	rm -f -o find *.o