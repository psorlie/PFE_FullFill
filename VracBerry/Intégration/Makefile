all: test_Postman test_Proxy test_Dispenser_Manager

test_Postman: test_Postman.c Postman.c cJSON.c DispenserManager.c Dispenser.c Proxy.c Date.c MessageToSend.c Product.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread -lm -lrt

test_Proxy: test_Proxy.c Postman.c Proxy.c cJSON.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread -lm

test_Dispenser_Manager: main.c DispenserManager.c Dispenser.c Postman.c cJSON.c Proxy.c Date.c MessageToSend.c Product.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread -lm -lrt

clean:
	rm test_Proxy test_Postman test_Dispenser_Manager