#!/bin/bash

echo -n "filename (e.g. n100): "
read filename
echo -n "dead space ratio (e.g. 0.2): "
read dsr

for i in {1..100000};
do

    ../bin/hw3 "../testcases/${filename}.hardblocks" "../testcases/${filename}.nets" "../testcases/${filename}.pl" "../output/${filename}_${dsr}.floorplan" "${dsr}" $RANDOM;

done

# Revise ./ArgParser/argParser.h & ./ArgParser/argParser.cc
# Revise ./Module/module.h
# Revise ./SAfloorplan/sa.cc
# Revise ./OutputWriter/outputWriter.h & ./OutputWriter/outputWriter.cc
# Revise main.cpp
# Don't forget to $make