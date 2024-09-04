# Makefile to compile the vmsim executable

CPP=g++
CFLAGS=-Wall -g

vmsim: input.o main.o
	$(CPP) -o vmsim input.o main.o $(CFLAGS)

clean:
	-rm -f *.o vmsim

input.o: input.cpp
	$(CPP) $(CFLAGS) -c input.cpp

main.o: main.cpp
	$(CPP) $(CFLAGS) -c main.cpp