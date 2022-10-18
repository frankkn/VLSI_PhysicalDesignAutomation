#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <cmath> 

std::unordered_map<int,std::pair<int,int>> Cells;
std::unordered_map<int,std::unordered_set<int>> CellArray;
std::unordered_map<int,std::unordered_set<int>> NetArray;

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

}A, B;

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

int calCutSize()
{
  int cut_cnt = 0;
  for(const auto& net: NetArray)
  {
    bool first_in_A = true;
    for(auto it = net.second.begin(); it != net.second.end(); ++it)
    {
      if(it == net.second.begin())
      {
        if(B.isIn(*it)) first_in_A = false;
      }
      if(first_in_A & B.isIn(*it))
      {
        ++cut_cnt;
        break;
      }
      else if(!first_in_A & A.isIn(*it))
      {
        ++cut_cnt;
        break;
      }
    }
  }
  return cut_cnt;
}


int calCellGain(int cell_num)
{
  int gain = 0;
  for(const auto& net: CellArray[cell_num])
  {
    int from = 0, to = 0;
    for(const auto& cell : NetArray[net]){
      if(A.isIn(cell))
      {
        ++from;
      }
      else
      {
        ++to;
      }
    }
    if(from == 1) ++gain;
    if(to == 0) --gain;
  }
  return gain;
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
  for(int i = 1; i <= 10; ++i)
    std::cout << "Cell Gain " << i << " = "<< calCellGain(i) << "\n";

  // TODO 4: Compute Cell Gains and build bucket list


  // Check balance or not
  // if(fabs(A.getArea() - B.getArea()) - (A.getArea() + B.getArea()) / 10 < 0)  { std::cout <<"YES";  }
  // else  { std::cout<<"NO";  }

  return 0;
}