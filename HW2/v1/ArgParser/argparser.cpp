#pragma once
#include "../Module/module.cpp"
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <iostream>
using namespace std;

unordered_map<string, Cell*> cellMap;

class Input;
class ArgParser
{
  private:
    // int setAsize;
    vector<Cell*> cellArray;
    vector<Net*> netArray;

  public:
    ArgParser() {}
    Input* parse(char *argv[]);

};

class Input
{
  public:
    // int setAsize;
    vector<Cell*> cellArray;
    vector<Net*> netArray;

    Input(vector<Cell*> cellArray, vector<Net*> netArray):cellArray(cellArray), netArray(netArray) {}
};

Input* ArgParser::parse(char *argv[])
{
  ifstream in_cell(argv[1]);

  string cell_name;
  int size_a, size_b;

  while(in_cell >> cell_name >> size_a >> size_b)
  {
    vector<int> cellArea;
    cellArea.emplace_back(size_a);
    cellArea.emplace_back(size_b);
    Cell* cell = new Cell(cell_name, move(cellArea));
    cellArray.emplace_back(cell);
    cellMap.emplace(cell_name, cell);
  }

  ifstream in_nets(argv[2]);
  string net_name, tmp;
  while(in_nets >> tmp >> net_name >> tmp)
  {
    Net* net = new Net(net_name);
    netArray.emplace_back(net);
    while(in_nets >> cell_name)
    {
      if(cell_name == "{")  continue;
      else if(cell_name == "}") break; 
      else
      {
        auto cell = cellMap.at(cell_name);
        net->cells.emplace_back(cell);
        cell->nets.emplace_back(net);
      }
    }
  }
  Input* input = new Input(this->cellArray, this->netArray);
  return input;
}