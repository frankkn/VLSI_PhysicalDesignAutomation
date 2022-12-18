#include "./outputwriter.h"
#include <fstream>
#include <iostream>
using namespace std;

void OutputWriter::WriteResult(char argv[])
{
  string auxPath = string(argv);
  string outputPath = auxPath.erase(0, auxPath.find_last_of('/')+1).erase(auxPath.find_last_of('.'), auxPath.size()-1);
  ofstream out("../output/" + outputPath + ".result");
  for(auto &cell: input->cells)
  {
    out << cell->name << " " << static_cast<int>(cell->bestX) << " " << static_cast<int>(cell->bestY) << "\n";
  }
  for(auto &terminal: input->terminals)
  {
    out << terminal->name << " " << static_cast<int>(terminal->x) << " " << static_cast<int>(terminal->y) << "\n";
  }
}
