#pragma once
#include <vector>
#include <string>
using namespace std;

struct Node 
{
  int type; // 0:cell 1:terminal
  string name;
  int width, height;
  int e; // weight
  double x, y; // downleft coordinate
  double bestX, bestY;

  Node
  (
    int const &type, string const &name, int const &width, int const &height, int const &e,
    double const &x = 0.0, double const &y = 0.0, double const &bestX = 0.0, double const &bestY =0.0
  ):type(type), name(name), width(width), height(height), e(width), x(x), y(y), bestX(0.0), bestY(0.0) {}
};

struct SubRow 
{
  int leftX, rightX;
  int capacity;
  Cluster* lastCluster;

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

struct Cluster
{
  double x_c, e_c, q_c, w_c; // pos, weight, used to give OPT pos for cell 1, width
  vector<Node*> nodes; 
  Cluster* prevC;

  Cluster(double const &x_c, double const &e_c, double const &q_c, double const &w_c, Cluster* prevC)
    :x_c(x_c), e_c(e_c), q_c(q_c), w_c(w_c), prevC(nullptr) {}
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