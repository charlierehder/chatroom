CC = gcc
CFLAGS  =  -g -pthread 

client: client.o common.o
	$(CC) $(CFLAGS) -o client client.o common.o

server: server.o common.o vector.o
	$(CC) $(CFLAGS) -o server server.o common.o vector.o

client.o: client.c common.h
	$(CC) $(CFLAGS) -c client.c

server.o: server.c common.h
	$(CC) $(CFLAGS) -c server.c

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c common.h

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c vector.h

.PHONY: clean

clean:
	rm -f server.o client.o common.o vector.o