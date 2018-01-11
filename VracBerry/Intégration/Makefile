all: test_Postman test_Proxy

test_Postman: test_Postman.c Postman.c cJSON.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread

test_Proxy: test_Proxy.c Postman.c Proxy.c cJSON.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread

clean:
	rm test_Proxy test_Postman