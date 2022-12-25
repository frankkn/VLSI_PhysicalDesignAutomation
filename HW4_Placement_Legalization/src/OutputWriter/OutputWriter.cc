#include <fstream>
#include "OutputWriter.h"
using namespace std;

void OutputWriter::WriteResult(char argv[])
{
  string auxPath = string(argv);
  string outputPath = auxPath.erase(0, auxPath.find_last_of('/')+1).erase(auxPath.find_last_of('.'), auxPath.size()-1);
  ofstream out("../output/" + outputPath + ".result");
  for(auto &cell: input->cellList)
  {
    out << cell->name << " " << static_cast<int>(cell->x_final) << " " << static_cast<int>(cell->y_final) << "\n";
  }
  for(auto &terminal: input->terminalList)
  {
    out << terminal->name << " " << static_cast<int>(terminal->x_global) << " " << static_cast<int>(terminal->y_global) << "\n";
  }
}

