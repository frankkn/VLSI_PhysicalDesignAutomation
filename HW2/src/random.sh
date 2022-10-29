#!/bin/bash


for i in {1..100000};
do

    ./main ../testcases/p2-1.cells ../testcases/p2-1.nets $RANDOM;

done