CC = g++
OPT = -O3
LIBS = -std=c++17
UDLIBS = ./Placer/Placer.cc ./Module/Module.cc ./OutputWriter/OutputWriter.cc

all: main

main: main.cpp
	rm -f ../bin/hw5
	rm -f ../output/*
	$(CC) -o ../bin/hw5 main.cpp $(UDLIBS) $(OPT) $(LIBS)

clean:
	rm -f ../bin/hw5
	rm -f ../output/*