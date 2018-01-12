all: sender client

sender: send.c server.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread

client: client.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread

clean:
	rm client server