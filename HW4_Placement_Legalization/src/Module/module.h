#pragma once
#include <vector>
#include <string>
using namespace std;

struct Node 
{
  int type; // 0:cell 1:terminal
  string name;
  int width, height;
  double x, y; // downleft coordinate

  Node(int const &type, string const &name, int const &width, int const &height, double const &x = 0.0, double const &y = 0.0)
    :type(type), name(name), width(width), height(height), x(x), y(y) {}
};

struct SubRow 
{
  int leftX, rightX;
  int capacity;

  SubRow(int leftX, int rightX):leftX(leftX), rightX(rightX), capacity(rightX-leftX) {}
};

struct CoreRow 
{
  double x, y; // downleft coordinate
  int height, siteWidth, siteNum;
  vector<SubRow*> subrows;

  CoreRow(int x, int y, int height, int siteWidth, int siteNum)
    :x(x), y(y), height(height), siteWidth(siteWidth), siteNum(siteNum) {}
};

struct Block 
{
  vector<CoreRow*> block;

  Block() {}
};

struct Input 
{
  int maxDisplacement;
  vector<Node*> cells, terminals;
  vector<CoreRow*> block;

  Input(int maxDisplacement, vector<Node*> cells, vector<Node*> terminals, vector<CoreRow*> block)
    :maxDisplacement(maxDisplacement), cells(cells), terminals(terminals), block(block) {}
};