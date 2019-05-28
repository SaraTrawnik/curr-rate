main: main.c str.o
	gcc -I/usr/include/json-c/ main.c -lcurl -ljson-c str.o -o curr-rate
str: str.c
	gcc str.c

clean:
	rm curr-rate
