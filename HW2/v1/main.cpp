#include <iostream>
#include "ArgParser/argparser.cpp"
#include "Module/module.cpp"
#include "FMpartition/partition.cpp"
using namespace std;


int main(int argc, char *argv[])
{
  //Step1: Parse argv and Construct cellArray and netArray
  ArgParser AP;
  Input* input = AP.parse(argv);

  FM_partition fmPartition(input);
  
  return 0;
}
