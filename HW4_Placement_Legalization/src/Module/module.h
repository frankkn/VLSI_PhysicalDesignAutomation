#pragma once
#include <vector>
#include <string>
using namespace std;

struct Node
{
  string name;
  int width, height;
  double x, y;

  Node(string const &name, int const &width, int const &height, double const &x = 0.0, double const &y = 0.0)
    :name(name), width(width), height(height), x(x), y(y) {}
};

struct Row
{
  int width, height;
  double x, y;

  Row() {}
};

struct Block
{
  vector<Row*> rows;

  Block() {}
};

struct Input
{
  int maxDisplacement;
  vector<Node*> cells, terminals;
  vector<Block*> blocks;

  Input(int maxDisplacement, vector<Node*> cells, vector<Node*> terminals, vector<Block*> blocks)
    :maxDisplacement(maxDisplacement), cells(cells), terminals(terminals), blocks(blocks) {}
};