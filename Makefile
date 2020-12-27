all:
	gcc -g src/main.c src/conf.c src/event.c -o main
release:
	gcc -g src/main.c src/event.c src/conf.c
