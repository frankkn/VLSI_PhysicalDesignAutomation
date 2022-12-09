#pragma once
#include <unordered_map>
#include <vector>
#include "../Module/module.h"
using namespace std;

class ArgParser
{
  int maxDisplacement;

  vector<Node*> cells, terminals;
  vector<CoreRow*> block;
  unordered_map<string, Node*> NodeTable;

  void readAux(string const & auxPath, string &nodePath, string &plPath, string &sclPath);
  void readNode(string const & nodePath);
  void readPl(string const & plPath);
  void readScl(string const & filePath);

  public:
    ArgParser() {}
    Input* readFile(char argv[]);
};
