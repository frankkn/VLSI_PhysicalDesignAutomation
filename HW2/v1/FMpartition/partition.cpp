#include "../ArgParser/argparser.cpp"
#include "../Module/module.cpp"
#include <cmath>
using namespace std;

class FM_partition
{
  private:
    Input* input;
    BucketList bucketList;
    vector<Set> sets;
    
  public:
    FM_partition (Input* input):input(input) {}
    void InitPartition();
    void calNetSetCellCnt();
    int calCutSize();
};

void FM_partition::InitPartition()
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
void FM_partition::calNetSetCellCnt()
{
  for(auto& net:input->netArray)
  {
    for(const auto& cell: net->cells)
    {
      ++net->setCellCnt[cell->set];
    }
  }
}

int FM_partition::calCutSize()
{
  calNetSetCellCnt();
  int cut_cnt = 0;
  for(auto net: input->netArray)
  {
    if(net->setCellCnt[0] > 0 && net->setCellCnt[1] > 0)
    {
      ++cut_cnt;
    }
  }
  return cut_cnt;
}

