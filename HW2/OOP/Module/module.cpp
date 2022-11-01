#include "module.hpp"
using namespace std;

void Set::addCell(Cell* cell, int num)
{
  cells.insert(cell);
  area += cell->cellArea[num];
}

void Set::deleteCell(Cell* cell, int num)
{
  cells.erase(cell);
  area -= cell->cellArea[num];
}
