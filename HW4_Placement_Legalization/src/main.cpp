#include <iostream>
#include "./ArgParser/argParser.h"
#include "./Abacus/abacus.h"
using namespace std;

// ../bin/hw4 ../testcase/adaptec1/adaptec1.aux

int main(int argc, char *argv[])
{
  ArgParser AP;
  auto input = AP.readFile(argv[1]);
  
  AbacusLegalizer AL(input);
  // AL.run();

  return 0;
}