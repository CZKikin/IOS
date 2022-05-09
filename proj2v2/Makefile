CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -pthread -lrt -g #-std=gnu99
BIN=proj2
OBJ=main.o vodik.o kyslik.o
DEPS=vodik.h kyslik.h

$(BIN): $(OBJ) $(DEPS) 
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
clean:
	rm proj2 *.o *.h.gch
