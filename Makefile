CC=gcc
CFLAGS=-g -pthread
SRC=src

client: client.o common.o
	$(CC) $(CFLAGS) -o client client.o common.o

server: server.o common.o vector.o
	$(CC) $(CFLAGS) -o server server.o common.o vector.o

client.o: $(SRC)/client.c
	$(CC) $(CFLAGS) -c $(SRC)/client.c

server.o: $(SRC)/server.c
	$(CC) $(CFLAGS) -c $(SRC)/server.c

common.o: $(SRC)/common.c
	$(CC) $(CFLAGS) -c $(SRC)/common.c

vector.o: $(SRC)/vector.c
	$(CC) $(CFLAGS) -c $(SRC)/vector.c

.PHONY: clean

clean:
	rm -f server.o client.o common.o vector.o