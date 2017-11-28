EXEC= client
CC = gcc
LD = $(CC)

CCFLAGS=-g -Wall
#-D_REENTRANT -D_POSIX_C_SOURCE=199309L -g -std=c99
LDFLAGS=-lpthread -lwebsockets
#-lrt

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

# Compilation rules
all: $(EXEC)

$(EXEC): $(OBJ)
	$(LD) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -Wall $(CCFLAGS) -c $< -o $@

# Clean the project
clean:
	rm *.o $(EXEC)





#all: server client

#server: server.c
	#gcc -g -Wall $< -o $@ `pkg-config libwebsockets --libs --cflags`
#	gcc -g -Wall $< -o $@  -lpthread -lwebsockets
#client: client.c  generate_percent.c
	#gcc -g -Wall $< -o $@ `pkg-config libwebsockets --libs --cflags`
#	gcc -g -Wall $< -o $@ -lpthread -lwebsockets

#clean:
#	rm -f server
#	rm -rf server.dSYM
#	rm -f client
#	rm -rf client.dSYM
