all: VracBerry_Server

VracBerry_Server: main.c DispenserManager.c Dispenser.c Postman.c cJSON.c Proxy.c Date.c MessageToSend.c Product.c Postman_UART.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread -lm -lrt -D_POSIX_C_SOURCE=200112L

clean:
	rm VracBerry_Server
