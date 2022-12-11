#include <algorithm>
#include <fstream>

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
  AL.cutSubRow();
  sort(input->cells.begin(), input->cells.end(), [&](auto &a, auto &b){ return a->x < b->x; });

  // ofstream out("./ta.txt");
  // for(auto cell: input->cells)
  // {
  //   int rowIdx = AL.getRowIdx(cell);
  //   auto row = input->block.at(rowIdx);
  //   int subrowIdx = AL.getSubRowIdx(row, cell);
  //   out << "Cell name: " << cell->name << " Row idx: " << rowIdx << " Subrow Idx: " << subrowIdx << endl;
  // }

  ofstream out("./me2.txt");
  for(auto cell:input->cells)
  {
    int rowIdx = -1;
    AL.locateCellCorerow(cell, rowIdx);
    auto row = input->block.at(rowIdx);
    int subrowIdx = -1;
    AL.locateCellSubrow(row, cell, subrowIdx);
    out << "Cell name: " << cell->name << " Row idx: " << rowIdx << " Subrow Idx: " << subrowIdx << endl;
  }
  
  // AL.run();

  return 0;
}