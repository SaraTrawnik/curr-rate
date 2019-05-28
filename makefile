main: main.c
	gcc -I/usr/include/json-c/ main.c -lcurl -ljson-c -o curr-rate
clean:
	rm curr-rate
