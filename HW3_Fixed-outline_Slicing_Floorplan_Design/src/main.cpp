#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "./ArgParser/argparser.h"
#include "./Module/module.h"
#include "./SAfloorplan/sa.h"
#include "./Clock/clock.h"
using namespace std;

void WriteResult(Input* input, string filepath, int WL)
{
  ofstream output(filepath);

  output << "Wirelength " << WL << "\n";
  output << "Blocks" << "\n";
  for(auto& hb: input->HBList)
  {
    output << hb->name << " " << hb->downleft_x << " " << hb->downleft_y << " " << hb->rotated << "\n"; 
  }
}

int main(int argc, char *argv[])
{
  Clock clock(10*60 - 6);
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
  OW->WriteResult(argv[4]);
  clock.EndClock("Output time");

  clock.EndClock("Total time");

  clock.PrintDuration("Input time");
  clock.PrintDuration("SA time");
  clock.PrintDuration("Output time");
  clock.PrintDuration("Total time");

  return 0;
}