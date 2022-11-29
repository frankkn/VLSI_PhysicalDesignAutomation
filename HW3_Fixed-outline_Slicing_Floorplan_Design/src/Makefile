CC = g++
OPT = -O3
# LIBS = -static-libstdc++
LIBS = -std=c++17

all: main

main: main.cpp
	$(CC) -o ../bin/hw3 main.cpp ./ArgParser/argParser.cc ./Module/module.cc ./SAfloorplan/sa.cc ./Clock/clock.cc ./OutputWriter/outputWriter.cc $(OPT) $(LIBS)

clean:
	rm -f ../bin/hw3