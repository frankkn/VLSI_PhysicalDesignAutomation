#include <unordered_map>
#include <vector>
#include "../Module/module.h"
using namespace std;

class ArgParser
{
  int maxDisplacement;

  vector<Node*> cells, terminals;
  vector<Block*> blocks;
  unordered_map<string, Node*> NodeTable;

  void ReadAux(string const & auxPath, string &nodePath, string &plPath, string &sclPath);
  void ReadNode(string const & nodePath);
  void ReadPl(string const & plPath);
  void ReadScl(string const & filePath);

  public:
    ArgParser() {}
    // Input* ReadFile(char *argv);
    void ReadFile(char *argv);
};
