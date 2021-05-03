CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -lpthread -lrt -g

all: proj2

proj2: main.c shMem.o
	$(CC) $(CFLAGS) $^ -o $@

shMem.o: shMem.c shMem.h
	$(CC) -c $(CFLAGS) $^ 

clean:
	rm proj2 *.o *.h.gch

test: testik.c
	$(CC) $(CFLAGS) $^ -o $@
