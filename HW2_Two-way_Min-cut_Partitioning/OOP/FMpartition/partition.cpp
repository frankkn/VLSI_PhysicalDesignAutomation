#include "partition.hpp"
#include <cmath>
using namespace std;

void FM::InitPartition()
{
  for(auto& cell: input->cellArray)
  {
    sets[0].addCell(cell, 0);
  }
  for(auto& cell: input->cellArray)
  {
    int diff_area = fabs(sets[0].area - sets[1].area);
    int total_area = sets[0].area + sets[1].area;
    bool balance = diff_area - total_area/10 < 0? true:false;
    if(!balance)
    {
      cell->set = 1;
      sets[0].deleteCell(cell, 0);
      sets[1].addCell(cell, 1); 
    }
  }
}

// Calculate # of Nets' cells' in set0 and set1
void FM::calNetGroupCnt()
{
  for(auto& net:input->netArray)
  {
    for(const auto& cell: net->cells)
    {
      ++net->setCnt[cell->set];
    }
  }
}

int FM::calCutSize()
{
  calNetGroupCnt();
  int cut_cnt = 0;
  for(auto net: input->netArray)
  {
    if(net->setCnt[0] > 0 && net->setCnt[1] > 0)
    {
      ++cut_cnt;
    }
  }
  return cut_cnt;
}

void FM::InitGain()
{
  for(auto cell: input->cellArray)
  {
    cell->locked = false;
    cell->gain = 0;
    for(auto net: cell->nets)
    {
      if(net->setCnt[cell->set] == 1)
      {
        ++cell->gain;
      }
      if(net->setCnt[!cell->set] == 0)
      {
        --cell->gain;
      }
    }
  }
}

