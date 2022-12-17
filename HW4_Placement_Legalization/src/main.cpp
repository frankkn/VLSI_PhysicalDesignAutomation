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

  // [DEBUG] cutSubRow() correct ?
  // ofstream out("./me2.txt");
  // for(auto cell:input->cells)
  // {
  //   int rowIdx = -1;
  //   AL.locateCellCorerow(cell, rowIdx);
  //   auto row = input->block.at(rowIdx);
  //   int subrowIdx = -1;
  //   AL.locateCellSubrow(row, cell, subrowIdx);
  //   out << "Cell name: " << cell->name << " Row idx: " << rowIdx << " Subrow Idx: " << subrowIdx << endl;
  // }

  // [DEBUG] cell o0 info ?
  // auto cell = input->cells[0];
  // int rowIdx = -1;
  // AL.locateCellCorerow(cell, rowIdx);
  // cout << "Cell name: " << input->cells[0]->name 
  // << " Row Idx: " << rowIdx << endl;
  // for(auto subrow: input->block[rowIdx]->subrows)
  // {
  //   cout << subrow->leftX << "," << subrow->rightX << "," << subrow->capacity << endl;
  // }

  // [DEBUG] placeTrialRow correct ?
  // ofstream out("me.txt");
  // for(int i = 0; i < 1000; ++i)
  // {
  //   double cBest {numeric_limits<double>::max()};
  //   int bestCorerowIdx = -1;
  //   AL.locateCellCorerow(input->cells[i], bestCorerowIdx);
  //   int bestSubrowIdx = -1;
  //   AL.locateCellSubrow(input->block[bestCorerowIdx], input->cells[i], bestSubrowIdx);
  //   out << "Cell name: " << input->cells[i]->name << endl;
  //   out << "Before X/Y: " << input->cells[i]->x <<"," << input->cells[i]->y<< endl;
  //   out << "Before bestX/bestY: : " << input->cells[i]->bestX <<"," << input->cells[i]->bestY<< endl;
  //   AL.placeTrialRow(input->cells[i], bestCorerowIdx, bestSubrowIdx);
  //   out << "After X/Y: " << input->cells[i]->x <<"," << input->cells[i]->y<< endl;
  //   out << "After: bestX/bestY: " << input->cells[i]->bestX <<"," << input->cells[i]->bestY<< endl;
  //   out << "Cost: " << AL.determineCost(input->cells[i]) << endl;
  //   out << "--------------------------------------------------------" << endl;
  // }

  // AL.abacusDP();
  // AL.run();

  return 0;
}