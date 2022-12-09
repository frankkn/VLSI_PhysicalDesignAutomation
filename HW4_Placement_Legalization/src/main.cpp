#include <iostream>
#include "./ArgParser/argParser.h"
using namespace std;

// ../bin/hw4 ../testcase/adaptec1/adaptec1.aux

int main(int argc, char *argv[])
{
  ArgParser AP;
  // auto input = AP.ReadFile(argv[1]);
  AP.ReadFile(argv[1]);

  return 0;
}