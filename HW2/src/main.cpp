#include <algorithm>
#include <cmath>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

std::unordered_map<int,std::pair<int,int>> Cells;
std::unordered_map<int,std::unordered_set<int>> CellArray;
std::unordered_map<int,std::unordered_set<int>> NetArray;
std::unordered_map<int, bool> LockedCells;
std::unordered_map<int, int> GainTable;
std::unordered_set<int> result_A;
std::unordered_map<int, std::pair<int,int>> netGroupCnt;

bool CanMove(int cell_num);

class Set
{
  private:
    int area;
    std::unordered_set<int> cellSet;
  public:
    Set():area(0), cellSet{} {}
    void addCell(int cell_name, int size);
    void deleteCell(int cell_name, int size);
    int getArea() { return area; }
    int getSize() { return cellSet.size(); };
    bool isIn(int cell_num) { return cellSet.count(cell_num); }
    std::unordered_set<int>& getCellSet() { return cellSet; };
}A, B;

class Cluster
{
  private:
    // int bucket_size;
    std::map<int, std::unordered_set<int>, std::greater<int>> GainList;
  public:
    Cluster() {}
    // int getSize() { return bucket_size; };
    void insertCell(int cell_name);
    void removeCell(int cell_name);
    void moveToOtherSide();
    int getBaseCell();
    std::map<int, std::unordered_set<int>, std::greater<int>> getGainList() { return GainList; } ;
}BucketList;

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
  GainList[cur_gain].insert(cell_name);
  // ++bucket_size;
}

int Cluster::getBaseCell()
{
  for(auto& [gain, cells]: GainList)
  {
    if(cells.size() == 0)
    {
      continue;
    }
    else
    {
      for(auto& cell: cells)
      {
        if(!LockedCells[cell] && CanMove(cell))
        {
          return cell;
        }
      }
    }
  }
  return -1;
}

void Cluster::removeCell(int cell_name)
{
  int cur_gain = GainTable[cell_name];
  GainList[cur_gain].erase(cell_name);
  // --bucket_size;
}

void calNetGroup()
{
  for(auto& [net_name, cells]: NetArray)
  {
    netGroupCnt[net_name].first = netGroupCnt[net_name].second = 0;
    for(auto& cell: cells)
    {
      if(A.isIn(cell))
      {
        netGroupCnt[net_name].first++;
      }
      else
      { 
        netGroupCnt[net_name].second++;
      }
    }
  }
}

int calCutSize()
{
  calNetGroup();
  int cut_cnt = 0;
  for(auto& [net_name, cells]:NetArray)
  {
    if(netGroupCnt[net_name].first != 0 && netGroupCnt[net_name].second != 0)
    {
      ++cut_cnt;
    }
  }
  return cut_cnt;
}

void InitGainTable()
{	
  calNetGroup();
  for(auto& [cell_name, nets]:CellArray)
  {
    if(A.isIn(cell_name))
    {
      for(auto& net:nets)
      {
        if(netGroupCnt[net].first == 1) ++GainTable[cell_name];
        if(netGroupCnt[net].second == 0) --GainTable[cell_name];
      }
    }
    else 
    {
      for(auto& net:nets)
      {
        if(netGroupCnt[net].second == 1) ++GainTable[cell_name];
        if(netGroupCnt[net].first == 0) --GainTable[cell_name];
      }
    }
  }
}

void InitBucketList()
{
  for(auto& [cell_name, cell_gain]: GainTable)
  {
    BucketList.insertCell(cell_name);
  }
}

void updateGain(int base)
{
  LockedCells[base] = true;
  // Base is in Set 0
  if(A.isIn(base))
  {
    A.deleteCell(base, Cells[base].first);
    B.addCell(base, Cells[base].second);
    for(auto& net: CellArray[base])
    {
      // To == 0
      if(netGroupCnt[net].second == 0)
      {
        for(auto& cell:NetArray[net])
        {
          if(cell != base && !LockedCells[cell])
          {
            BucketList.removeCell(cell);
            ++GainTable[cell];
            BucketList.insertCell(cell);
          }
        }
      }
      // To == 1
      else if(netGroupCnt[net].second == 1)
      {
        for(auto& cell: NetArray[net])
        {
          if(cell != base && !LockedCells[cell] && B.isIn(cell))
          {
            BucketList.removeCell(cell);
            --GainTable[cell];
            BucketList.insertCell(cell);
          }
        }
      }

      // From--, To++
      --netGroupCnt[net].first;
      ++netGroupCnt[net].second;

      // From == 0
      if(netGroupCnt[net].first == 0)
      {
        for(auto& cell: NetArray[net])
        {
          if(cell != base && !LockedCells[cell] && B.isIn(cell))
          {
            BucketList.removeCell(cell);
            --GainTable[cell];
            BucketList.insertCell(cell);
          }
        }
      }
      // From == 1
      else if(netGroupCnt[net].first == 1)
      {
        for(auto& cell: NetArray[net])
        {
          if(cell != base && !LockedCells[cell] && A.isIn(cell))
          {
            BucketList.removeCell(cell);
            ++GainTable[cell];
            BucketList.insertCell(cell);
          }
        }
      }
    }
  }
  // Base is in Set 1
  else
  {
    B.deleteCell(base, Cells[base].second);
    A.addCell(base, Cells[base].first);
    for(auto& net: CellArray[base])
    {
      // To == 0
      if(netGroupCnt[net].first == 0)
      {
        for(auto& cell:NetArray[net])
        {
          if(cell != base && !LockedCells[cell])
          {
            BucketList.removeCell(cell);
            ++GainTable[cell];
            BucketList.insertCell(cell);
          }
        }
      }
      // To == 1
      else if(netGroupCnt[net].first == 1)
      {
        for(auto& cell: NetArray[net])
        {
          if(cell != base && !LockedCells[cell] && A.isIn(cell))
          {
            BucketList.removeCell(cell);
            --GainTable[cell];
            BucketList.insertCell(cell);
          }
        }
      }

      // From--, To++
      --netGroupCnt[net].second;
      ++netGroupCnt[net].first;

      // From == 0
      if(netGroupCnt[net].second == 0)
      {
        for(auto& cell: NetArray[net])
        {
          if(cell != base && !LockedCells[cell] && A.isIn(cell))
          {
            BucketList.removeCell(cell);
            --GainTable[cell];
            BucketList.insertCell(cell);
          }
        }
      }
      // From == 1
      else if(netGroupCnt[net].second == 1)
      {
        for(auto& cell: NetArray[net])
        {
          if(cell != base && !LockedCells[cell] && B.isIn(cell))
          {
            BucketList.removeCell(cell);
            ++GainTable[cell];
            BucketList.insertCell(cell);
          }
        }
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
  long long diff_area = llabs(tmp_A_area - tmp_B_area);
  long long total_area = (tmp_A_area + tmp_B_area) / 10;
  return diff_area - total_area < 0? true: false;
}

void UnlockAllCells()
{
  for(auto& [cell, state]: LockedCells)
  {
    state = false;
  }
}

int best_cutsize = 1e+9;

int fmProcess()
{
  int lock_num = 0;
  int partialSum = 0, maxPartialSum = 0;
  int cur_cutsize = calCutSize();
  while(lock_num < Cells.size())
  {
    int base = BucketList.getBaseCell();
    if(base != -1)
    {
      partialSum += GainTable[base];
      
      // std::cout << "Base cell's gain : " << GainTable[base] << std::endl;
      // std::cout << "Cut size by deduction : " << cur_cutsize << std::endl;
      // std::cout << "Cut size by function : " << calCutSize() << std::endl; 
      cur_cutsize -= GainTable[base];

      if(maxPartialSum < partialSum)
      {
        maxPartialSum = partialSum;
      }
      
      updateGain(base);

      if(cur_cutsize < best_cutsize)
      {
        best_cutsize = cur_cutsize;
        result_A = A.getCellSet();
      }
      // std::cout << "After, cut size by deduction  : " << cur_cutsize << std::endl;
      // std::cout << "After, cut size by function : " << calCutSize() << std::endl;
      // std::cout << "----------------------------------------------" << std::endl;
    }
    else
    {
      //UnlockAllCells();
      break;
    }
    lock_num++;
  }
  return best_cutsize;
  //return maxPartialSum;
}

void WriteResult(std::string filename, int best_cutsize)
{
  std::ofstream output(filename);

  output << "cut_size " << best_cutsize << std::endl;
  output << "A " << result_A.size() <<std::endl;
  for( auto &cell : result_A)
  {
    output << "c" << cell << std::endl;
  }
  
  std::unordered_set<int> result_B;
  for(auto& [cell_name, nets]:CellArray)
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
  auto begin = std::chrono::high_resolution_clock::now();

  std::ifstream fin_cell(argv[1]);
  std::ifstream fin_nets(argv[2]);

  // Step 1: Read Cell Area (size in Set A/Set B)
  std::string cell_name;
  int size_a, size_b;
  while(fin_cell >> cell_name >> size_a >> size_b)
  {
    Area_A += size_a;
    Area_B += size_b;
    cell_name.erase(0,1);
    int cell_num = stoi(cell_name);
    Cells[cell_num] = std::make_pair(size_a,size_b);
    GainTable[cell_num] = 0;
  }

  // Step 2: ruct NetArray and CellArray
  std::string tmp, net_name;
  while(fin_nets >> tmp >> net_name >> tmp)
  {
    int net_num = stoi(net_name.erase(0,1));
    while(fin_nets >> cell_name && cell_name[0] != '}')
    {
      int cell_num = stoi(cell_name.erase(0,1));
      NetArray[net_num].insert(cell_num);
      CellArray[cell_num].insert(net_num);
      LockedCells[cell_num] = false;
    }
  }

  // Step 3: Init Partition
  for(auto& [cell_name, size]:Cells)
  {
    A.addCell(cell_name, size.first);
  }
  for(auto& [cell_name, size]:Cells)
  {
    long long diff_area = llabs(A.getArea() - B.getArea());
    long long total_area = (A.getArea() + B.getArea()) / 10;
    bool balance = diff_area - total_area < 0? true: false;
    if(!balance)
    {
      A.deleteCell(cell_name, size.first);
      B.addCell(cell_name, size.second);
    }
  }

  // std::cout << "Size of Set A = " << A.getCellSet().size() << "\nSize of Set B = " << B.getCellSet().size() << "\n";
  // std::cout << "Cut size = " << calCutSize() << "\n";

  // int Pmax = 1e-6;
  // int pin;
  // for(auto& cell: CellArray)
  // {
  //   pin = 0;
  //   for(auto it = cell.second.begin(); it != cell.second.end(); ++it)
  //   {
  //     ++pin;
  //   }
  //   if(pin > Pmax)  Pmax = pin;
  // }
  // std::cout << "Pax = " << Pmax << "\n";

  // Step 4: Buildup GainTable and Bucketlist
  InitGainTable();
  InitBucketList();

  // // //Check Bucket list 
  // for(auto& [cell_gain, cells]:BucketList.getGainList())
  // {
  //   std::cout << "cell gain = " << cell_gain;
  //   for(auto it = cells.begin(); it != cells.end(); ++it)
  //   {
  //     std::cout << "cells" << *it << " ";
  //   }
  //   std::cout << "\n";
  // }

  // Check GainTable
  // for(auto& [cell_name, cell_gain]:GainTable)
  // {
  //   std::cout << "cell_name: " << cell_name << "cell_gain: " << cell_gain << "\n";
  // }

  // Step 5: FM process 
  // Move one cell with max gain from A to B 

  // std::cout << "Before FM, cut size = " << calCutSize() << "\n";
  // std::cout << "Before FM, Size of Set A = " << A.getSize() << "\n";
  // std::cout << "Before FM, Size of Set B = " << B.getSize() << "\n";
  // std::cout << "----------------------------------------" << "\n";

  fmProcess();

  // std::cout << "After FM, cut size = " << best_cutsize << "\n";
  // std::cout << "After FM, Size of Set A = " << result_A.size() << "\n";
  // std::cout << "After FM, Size of Set B = " << Cells.size() - result_A.size() << "\n";
  // std::cout << "----------------------------------------" << "\n";
  
  // int pass = 0;
  // while(true)
  // {
  //   ++pass;
  //   int maxPartialSum = fmProcess();
  //   if(maxPartialSum <= 0)
  //   {
  //     std::cout << "FM pass = " << pass << "\n";
  //     // std::cout << "After FM, Size of Set A = " << result_A.size() << "\n";
  //     // std::cout << "After FM, Size of Set B = " << Cells.size() - result_A.size() << "\n";
  //     std::cout << "After FM, Size of cut size = " << best_cutsize << "\n";
  //     break;
  //   }
  //   else
  //   {
  //     std::cout << "Max partial sum = " << maxPartialSum << "\n";
  //   }
  // }

  auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout<< "Time measured: "<<  elapsed.count() * 1e-9 << "seconds" << "\n";

  std::cout << best_cutsize << "\n";
  
  WriteResult(argv[3], best_cutsize);
  return 0;
}