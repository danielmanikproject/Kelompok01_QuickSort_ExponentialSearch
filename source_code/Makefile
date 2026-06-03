# Makefile untuk kompilasi Tugas Proyek Struktur Data Kelompok 01 PTIK-C 2026
CC = g++
CFLAGS = -Wall -std=c++11

all: main

main: main.o sorting.o searching.o
	$(CC) $(CFLAGS) -o main main.o sorting.o searching.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

sorting.o: sorting.cpp
	$(CC) $(CFLAGS) -c sorting.cpp

searching.o: searching.cpp
	$(CC) $(CFLAGS) -c searching.cpp

clean:
	rm -f *.o main