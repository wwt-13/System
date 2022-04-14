all:
	gcc main.c -E -o main.i
	gcc main.i -S -o main.S
	gcc main.S -c -o main.o
	gcc main.c test.c -o main
clean:
	del main.i
	del main.S
	del main.o
	del main.exe