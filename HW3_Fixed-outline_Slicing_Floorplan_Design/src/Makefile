CC = g++
OPT = -O3
# LIBS = -static-libstdc++
LIBS = -std=c++17

UDLIBS = ./ArgParser/argParser.cc ./Module/module.cc ./SAfloorplan/sa.cc ./Clock/clock.cc ./OutputWriter/outputWriter.cc ./MemoryUsage/mem.cc

all: main

main: main.cpp
	$(CC) -o ../bin/hw3 main.cpp $(UDLIBS) $(OPT) $(LIBS)

clean:
	rm -f ../bin/hw3
	rm -f ../output/*