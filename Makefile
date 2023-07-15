CC=g++
CFLAGS=-std=c++17 -Wall -Wextra -pedantic -O3 -static
LFLAGS=-lgmp

all: g2

g2: main.o ECC.o
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

main.o: main.cpp ECC.h
	$(CC) $(CFLAGS) -c $< -o $@

ECC.o: ECC.cpp ECC.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o program