all:
	gcc src/main.c src/conf.c src/event.c src/util.c \
		-o disorg-main -pedantic -Wall -g
release:
	gcc src/main.c src/event.c src/conf.c src/util.c \
		-o disorg-main
install: release
	cp disorg-main disorg.sh /usr/local/bin/
uninstall:
	rm /usr/local/bin/disorg-main
	rm /usr/local/bin/disorg.sh
