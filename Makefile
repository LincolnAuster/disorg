all:
	gcc src/cli.c src/conf.c src/event.c src/util.c \
		-o disorg-main -pedantic -Wall -g
test:
	gcc src/util.c src/conf.c tests/tests.c -o tests/test
	./tests/test
release:
	gcc src/cli.c src/event.c src/conf.c src/util.c \
		-o disorg-main
install: release
	cp disorg-main disorg.sh /usr/local/bin/
uninstall:
	rm /usr/local/bin/disorg-main
	rm /usr/local/bin/disorg.sh
