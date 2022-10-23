#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <map>
#include <functional>

std::unordered_map<int,std::pair<int,int>> Cells;
std::unordered_map<int,std::unordered_set<int>> CellArray;
std::unordered_map<int,std::unordered_set<int>> NetArray;
//std::map<int,std::unordered_set<int>,std::greater<int>> BucketList;
std::unordered_map<int, bool> LockedCells;
std::unordered_map<int, int> GainTable;
std::unordered_set<int> result_A;


class Set
{
  private:
    int area;
    std::unordered_set<int> cellSet;
  public:
    Set():area(0), cellSet{} {}
    void addCell(int cell_name, int size);
    void deleteCell(int cell_name, int size);
    int getArea()  { return area; }
    bool isIn(int cell_num) { return cellSet.count(cell_num); }
    std::unordered_set<int>& getCellSet() { return cellSet; };
}A, B;

class Cluster
{
  private:
    int bucket_size;
    std::map<int, std::unordered_set<int>, std::greater<int>> cellList;
  public:
    Cluster(): bucket_size(0) {}
    int getSize() { return bucket_size; };
    void insertCell(int cell_name);
    void removeCell(int cell_name);
    void moveToOtherSide();
    int getBaseCell();
    std::map<int, std::unordered_set<int>, std::greater<int>> getCellList() { return cellList; } ;
}Bucket_A, Bucket_B;

void Set::addCell(int cell_name, int size)
{
  cellSet.insert(cell_name);
  area += size;
} 

void Set::deleteCell(int cell_name, int size)
{
  cellSet.erase(cell_name);
  area -= size;
}

void Cluster::insertCell(int cell_name)
{
  int cur_gain = GainTable[cell_name];
  cellList[cur_gain].insert(cell_name);
  LockedCells[cell_name] = false;
  ++bucket_size;
}

int Cluster::getBaseCell()
{
  for(auto it = cellList.begin(); it != cellList.end(); )
  {
    if((*it).second.size() == 0)
    {
      ++it;
    }
    else
    {
      return *(*it).second.begin();
    }
  }
  return -1;
}

void Cluster::removeCell(int cell_name)
{
  int cur_gain = GainTable[cell_name];
  cellList[cur_gain].erase(cell_name);
  LockedCells[cell_name] = true;
  --bucket_size;
}

// int calCutSize()
// {
//   int cut_cnt = 0;
//   for(const auto& [net_num, cells]: NetArray)
//   {
//     bool in_A = false, in_B = false;
//     for(auto it = cells.begin(); it != cells.end(); ++it)
//     {
//       if(A.isIn(*it)) in_A = true;
//       else  in_B = true;
//       if(in_A && in_B)
//       {
//         ++cut_cnt;
//         break;
//       }
//     }
//   }
//   return cut_cnt;
// }

int calCutSize()
{
  int cut_cnt = 0;
  for(const auto& [net_name, cells]:NetArray)
  {
    int linkA = 0, linkB = 0;
    for(const auto& cell: cells)
    {
      if(A.isIn(cell))  ++linkA;
      else  ++linkB;
    }
    if(linkA != 0 && linkB != 0)
    {
      ++cut_cnt;
    }
  }
  return cut_cnt;
}

int calCellGain(int cell_num)
{
  int gain = 0;
  for(const auto& net: CellArray[cell_num])
  {
    int from = 1, to = 0;
    for(const auto& cell : NetArray[net]){
      if(cell != cell_num)
      {
        if(A.isIn(cell) != A.isIn(cell_num))
        {
          ++to;
        }
        else
        {
          ++from;
        }
      }
    }
    if(from == 1) ++gain;
    if(to == 0) --gain;
  }
  return gain;
}

void updateGain(int base)
{
  LockedCells[base] = true;
  for(const auto& net:CellArray[base])
  {
    int from = 1, to = 0;
    for(const auto& cell:NetArray[net])
    {
      if(cell != base)
      {
        if(A.isIn(cell) == A.isIn(base))  ++from;
        else ++to;
      }
    }
    if(to == 0)
    {
      for(const auto& cell:NetArray[net])
      {
        if(cell != base && !LockedCells[cell])
        {
          int cur_gain = GainTable[cell];
          if(A.isIn(base))
          {
            Bucket_A.removeCell(cell);
          }
          else
          {
            Bucket_B.removeCell(cell);
          }
          GainTable[cell] = ++cur_gain;
          if(A.isIn(base))
          {
            Bucket_A.insertCell(cell);
          }
          else
          {
            Bucket_B.insertCell(cell);
          }
        }
      }
    }
    else if(to == 1)
    {
      for(const auto& cell:NetArray[net])
      {
        if(A.isIn(base) != A.isIn(cell))
        {
          int cur_gain = GainTable[cell];
          if(A.isIn(base))
          {
            Bucket_B.removeCell(cell);
          }
          else
          {
            Bucket_A.removeCell(cell);
          }
          GainTable[cell] = --cur_gain;
          if(A.isIn(base))
          {
            Bucket_B.insertCell(cell);
          }
          else
          {
            Bucket_A.insertCell(cell);
          }
        }
        break;
      }
    }

    if(A.isIn(base))
    {
      auto [size_a, size_b] = Cells[base];
      Bucket_A.removeCell(base);
      A.deleteCell(base, size_a);
      B.addCell(base, size_b);
    }
    else
    {
      auto [size_a, size_b] = Cells[base];
      Bucket_B.removeCell(base);
      B.deleteCell(base, size_b);
      A.addCell(base, size_a);
    }

    from = 0, to = 1;
    for(const auto& cell:NetArray[net])
    {
      if(cell != base) // base is in To now
      {
        if(A.isIn(cell) == A.isIn(base))  ++to;
        else ++from;
      }
    }
    if(from == 0)
    {
      for(const auto& cell:NetArray[net])
      {
        if(cell != base && !LockedCells[cell])
        {
          int cur_gain = GainTable[cell];
          if(A.isIn(base))
          {
            Bucket_A.removeCell(cell);
          }
          else
          {
            Bucket_B.removeCell(cell);
          }
          GainTable[cell] = --cur_gain;
          if(A.isIn(base))
          {
            Bucket_A.insertCell(cell);
          }
          else
          {
            Bucket_B.insertCell(cell);
          }
        }
      }
    }
    else if(from == 1)
    {
      for(const auto& cell:NetArray[net])
      {
        if(A.isIn(base) != A.isIn(cell))
        {
          int cur_gain = GainTable[cell];
          if(A.isIn(base))
          {
            Bucket_B.removeCell(cell);
          }
          else
          {
            Bucket_A.removeCell(cell);
          }
          GainTable[cell] = ++cur_gain;
          if(A.isIn(base))
          {
            Bucket_B.insertCell(cell);
          }
          else
          {
            Bucket_A.insertCell(cell);
          }
        }
        break;
      }
    }
  }
}

bool CanMove(int cell_num)
{
  int tmp_A_area = A.getArea(), tmp_B_area = B.getArea();
  if(A.isIn(cell_num))
  {  
    tmp_A_area -= Cells[cell_num].first;
    tmp_B_area += Cells[cell_num].second;
  }
  else
  {
    tmp_B_area -= Cells[cell_num].second;
    tmp_A_area += Cells[cell_num].first;
  }
  int diff_area = fabs(tmp_A_area - tmp_B_area);
  int total_area = (tmp_A_area + tmp_B_area) / 10;
  return diff_area - total_area < 0? true: false;
}

int fmProcess()
{
  bool MoveAtoB = false, MoveBtoA = false;
  int partialSum = 0, maxPartialSum = 0;
  while(Bucket_A.getSize() > 0 && Bucket_B.getSize() > 0)
  {
    // first move A to B
    if(A.getArea() >= B.getArea() || Bucket_B.getSize() <= 0 || MoveAtoB )
    {
      // Bucket list still has unlocked cells
      while(Bucket_A.getSize() > 0)
      {
        int base = Bucket_A.getBaseCell();
        //std::cout << GainTable[base] << "!";
        if(CanMove(base))
        {
          // Reset so next time we Move B to A
          MoveAtoB = false;
          partialSum += GainTable[base];
          updateGain(base);
        }
        else
        {
          MoveBtoA = true;
          break;
        }
        if(maxPartialSum < partialSum)
        {
          maxPartialSum = partialSum;
          result_A = A.getCellSet();
        }
      }
    }
    if(A.getArea() < B.getArea() || Bucket_A.getSize() <= 0 || MoveBtoA )
    {
      while(Bucket_B.getSize() > 0)
      {
        int base = Bucket_B.getBaseCell();
        //std::cout << GainTable[base] << "!";
        if(CanMove(base))
        {
          MoveBtoA = false;
          partialSum += GainTable[base];
          updateGain(base);
        }
        else
        {
          MoveAtoB = true;
          break;
        }
        if(maxPartialSum < partialSum)
        {
          maxPartialSum = partialSum;
        }
      }
    }
  }
  return maxPartialSum;
}

void WriteResult(std::string filename, int best_cutsize)
{
  std::ofstream output(filename);

  output << "cut_size " << best_cutsize << std::endl;
  output << "A " << result_A.size() <<std::endl;
  for(const auto &cell : result_A)
  {
    output << "c" << cell << std::endl;
  }
  
  std::unordered_set<int> result_B;
  for(const auto& [cell_name, nets]:CellArray)
  {
    if(!result_A.count(cell_name))
    {
      result_B.insert(cell_name);
    }
  }

  output << "B " << result_B.size() << std::endl;
  for(auto &cell : result_B)
  {
    output << "c" << cell << std::endl;
  }
}

int Area_A = 0, Area_B = 0;

int main(int argc , char *argv[])
{
  std::ifstream fin_cell(argv[1]);
  std::ifstream fin_nets(argv[2]);

  // TODO 1:Read Cell Area (size in a/b)
  std::string cell_name;
  int size_a, size_b;
  while(fin_cell >> cell_name >> size_a >> size_b)
  {
    Area_A += size_a;
    Area_B += size_b;
    cell_name.erase(0,1);
    Cells[stoi(cell_name)] = std::make_pair(size_a,size_b);
  }

  // for(const auto& [x,y]:Cells)
  // {
  //   std::cout <<  x << " " << y.first << " " << y.second << "\n";
  // }

  // TODO 2:Construct NetArray and CellArray
  std::string tmp, net_name;
  while(fin_nets >> tmp >> net_name >> tmp)
  {
    int net_num = stoi(net_name.erase(0,1));
    while(fin_nets >> cell_name && cell_name[0] != '}')
    {
      int cell_num = stoi(cell_name.erase(0,1));
      NetArray[net_num].insert(cell_num);
      CellArray[cell_num].insert(net_num);
    }
  }

  // TODO 3:Init Partition
  for(const auto& [cell_name, size]:Cells)
  {
    A.addCell(cell_name, size.first);
  }
  for(const auto& [cell_name, size]:Cells)
  {
    int diff_area = fabs(A.getArea() - B.getArea());
    int total_area = (A.getArea() + B.getArea()) / 10;
    bool balance = diff_area - total_area < 0? true: false;
    if(!balance)
    {
      A.deleteCell(cell_name, size.first);
      B.addCell(cell_name, size.second);
    }
  }

  std::cout << "Area A = " << A.getArea() << "\nArea B = " << B.getArea() << "\n";
  std::cout << "Cut size = " << calCutSize() << "\n";

  // int Pmax = 1e-6;
  // int pin;
  // for(const auto& cell: CellArray)
  // {
  //   pin = 0;
  //   for(auto it = cell.second.begin(); it != cell.second.end(); ++it)
  //   {
  //     ++pin;
  //   }
  //   if(pin > Pmax)  Pmax = pin;
  // }
  // std::cout << "Pax = " << Pmax << "\n";

  // TODO 4: Compute init Cell Gains and build bucket list and GainTable
  for(const auto& [cell_name, nets]:CellArray)
  {
    GainTable[cell_name] = calCellGain(cell_name);
    //std::cout << "cell_name: " << cell_name << "cell_gain" << GainTable[cell_name] << "\n"; 
    if(A.isIn(cell_name))
    {
      Bucket_A.insertCell(cell_name);
    }
    else
    {
      Bucket_B.insertCell(cell_name);
    }
  }

  // Check Bucket list 
  // for(const auto& [cell_gain, cells]:Bucket_A.getCellList())
  // {
  //   std::cout << "cell gain = " << cell_gain;
  //   for(auto it = cells.begin(); it != cells.end(); ++it)
  //   {
  //     std::cout << "cells" << *it << " ";
  //   }
  //   std::cout << "\n";
  // }

  for(const auto& [cell_name, cell_gain]:GainTable)
  {
    std::cout << "cell_name: " << cell_name << "cell_gain: " << cell_gain << "\n";
  }

  //TODO 5: FM process, move one cell from A to B and then move one cell from B to A until no unlocked cells
  while(true)
  {
    int maxPartialSum = fmProcess();
    if(maxPartialSum > 0)
    {
      //std::cout << "maxPartialSum > 0!" << std::endl ;
    }
    else
    {
      std::cout << calCutSize() << std::endl;
      break;
    }
  }

  WriteResult(argv[3], calCutSize());
  return 0;
}