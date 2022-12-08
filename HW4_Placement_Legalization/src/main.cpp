#include <iostream>
#include "./ArgParser/argParser.h"
using namespace std;

int main(int argc, char *argv[])
{
  ArgParser AP;
  // auto input = AP.ReadFile(argv[1]);
  AP.ReadFile(argv[1]);

  return 0;
}