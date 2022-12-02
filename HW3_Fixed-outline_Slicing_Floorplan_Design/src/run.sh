#!/bin/bash

# echo -n "compile g++, run, and verify (y for yes, enter for skip):"
# read ans
# if [[ ${ans} == "y" ]]; then
#   echo -n "filename (e.g. n100):"
#   read filename
#   echo -n "dead space ratio (e.g. 0.2):"
#   read dsr
#   set -x
#   g++ -std=c++17 -o main.out main.cpp
#   ./main.out "../testcases/${filename}.hardblocks" "../testcases/${filename}.nets" "../testcases/${filename}.pl" "../output/${filename}.floorplan" "${dsr}"
#   ../verifier/verifier "../testcases/${filename}.hardblocks" "../testcases/${filename}.nets" "../testcases/${filename}.pl" "../output/${filename}.floorplan" "${dsr}"
#   set +x
# fi

echo -n "compile via make, run, and verify (y for yes, enter for skip): "
read ans
if [[ ${ans} == "y" ]]; then
  echo -n "filename (e.g. n100): "
  read filename
  echo -n "dead space ratio (e.g. 0.2): "
  read dsr
  set -x
# make clean
  make
  ../bin/hw3 "../testcases/${filename}.hardblocks" "../testcases/${filename}.nets" "../testcases/${filename}.pl" "../output/${filename}_${dsr}.floorplan" "${dsr}"
  ../verifier/verifier "../testcases/${filename}.hardblocks" "../testcases/${filename}.nets" "../testcases/${filename}.pl" "../output/${filename}_${dsr}.floorplan" "${dsr}"
  set +x
fi

# ../bin/hw3 ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/$n100.pl ../output/n100.floorplan 0.1
