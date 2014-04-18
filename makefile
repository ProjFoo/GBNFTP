all: checksum gremlin client server

client:

server: server.c debug
	gcc server.c -o server

gremlin: gremlin.c gremlin.h debug
	gcc -c gremlin.c -o gremlin.o

checksum: checksum.c checksum.h debug
	gcc -lssl -lcrypto -g -Wall -c checksum.c -o checksum.o

debug: debug.h
