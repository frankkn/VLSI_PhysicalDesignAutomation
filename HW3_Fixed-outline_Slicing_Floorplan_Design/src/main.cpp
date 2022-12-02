#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "./ArgParser/argParser.h"
#include "./Module/module.h"
#include "./SAfloorplan/sa.h"
#include "./Clock/clock.h"
#include "./MemoryUsage/mem.h"
using namespace std;

int main(int argc, char *argv[])
{
  Clock clock(10*60 - 3);
  clock.StartClock("Total time");
  
  clock.StartClock("Input time");
  ArgParser AP;
  auto input = AP.ReadFile(argv);
  clock.EndClock("Input time");
  
  clock.StartClock("SA time");
  SA SAFP(input, clock);
  OutputWriter* OW = SAFP.Run();
  clock.EndClock("SA time");

  clock.StartClock("Output time");
  // OW->WriteWLseed(argv[4]);
  OW->WriteResult(argv[4]);
  clock.EndClock("Output time");

  clock.EndClock("Total time");

  MemoryUsage MU;
  MU.PrintMemUsage();

  clock.PrintDuration("Input time");
  clock.PrintDuration("SA init time");
  clock.PrintDuration("SA area phase time");
  clock.PrintDuration("SA wirelength phase time");
  clock.PrintDuration("SA time");
  clock.PrintDuration("Output time");
  clock.PrintDuration("Total time");

  return 0;
}