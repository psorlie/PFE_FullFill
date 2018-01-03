all: test_Postman

test_Postman: test_Postman.c Postman.c
	gcc -Wall -o $@ $^ -lmosquitto -lpthread

clean:
	rm test_Postman