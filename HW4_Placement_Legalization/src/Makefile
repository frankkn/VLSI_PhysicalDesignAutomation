CC = g++
OPT = -O3
# LIBS = -static-libstdc++
LIBS = -std=c++17

UDLIBS = ./ArgParser/argParser.cc ./Module/module.cc 

all: main

main: main.cpp
	$(CC) -o ../bin/hw4 main.cpp $(UDLIBS) $(OPT) $(LIBS)

clean:
	rm -f ../bin/hw4
	rm -f ../output/*