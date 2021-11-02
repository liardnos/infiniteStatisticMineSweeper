
all:
	cd build && make -j 8 || clear && make

client: all
	./build/bin/r-type_client

_clear:
	clear

valgrindclient: _clear all
	valgrind --track-origins=yes ./build/bin/r-type_client
