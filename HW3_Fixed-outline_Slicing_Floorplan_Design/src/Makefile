CC = g++
OPT = -O3
# LIBS = -static-libstdc++
LIBS = -std=c++17

all: main

main: main.cpp
	$(CC) -o ../bin/hw3 main.cpp ./module/module.cc ./SAfloorplan/sa.cc $(OPT) $(LIBS)

clean:
	rm -f ../bin/hw3