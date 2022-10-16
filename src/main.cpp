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

};

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
  Set A, B;
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
  
  if(fabs(A.getArea() - B.getArea()) - (A.getArea() + B.getArea()) / 10 < 0)  { std::cout <<"YES";  }
  else  { std::cout<<"NO";  }


  return 0;
}