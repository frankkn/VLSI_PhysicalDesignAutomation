#include <unordered_map>
#include <vector>
#include "../Module/module.h"
using namespace std;

class ArgParser
{
  private:
    double dead_space_ratio;
    vector<HardBlock*> HBList;
    unordered_map<string, HardBlock*> HBTable;
    vector<net*> NetList;
    unordered_map<string, pin*> PinTable;

    void ReadHardBlock(string const & filepath);
    void ReadPin(string const & filepath);
    void ReadNet(string const & filepath);

  public:
    ArgParser() {}
    Input* ReadFile(char* argv[]);
};