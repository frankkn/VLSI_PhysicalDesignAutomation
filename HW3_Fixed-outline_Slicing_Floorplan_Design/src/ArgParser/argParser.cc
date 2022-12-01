#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include "argParser.h"
using namespace std;


void ArgParser::ReadHardBlock(string const & filepath)
{
  ifstream fin_hardblocks(filepath);
  string s;
  while(getline(fin_hardblocks, s))
  {
    if(s == "" || s[0] != 's') continue;
    stringstream ss(s);
    string name, tmp1, tmp2, x_0, y_0, x_1, y_1, x_2, y_2, x_3, y_3;
    while(ss >> name >> tmp1 >> tmp2 >> x_0 >> y_0 >> x_1 >> y_1 >> x_2 >> y_2 >> x_3 >> y_3)
    {
      x_0.erase(0,1).pop_back();x_1.erase(0,1).pop_back();x_2.erase(0,1).pop_back();x_3.erase(0,1).pop_back();
      int x0 = stoi(x_0), x1 = stoi(x_1), x2 = stoi(x_2), x3 = stoi(x_3);
      y_0.pop_back();y_1.pop_back();y_2.pop_back();y_3.pop_back();
      int y0 = stoi(y_0), y1 = stoi(y_1), y2 = stoi(y_3), y3 = stoi(y_3);

      int leftmost_x = min(x0, min(x1, min(x2, x3)));
      int rightmost_x = max(x0, max(x1, max(x2, x3)));
      int width = rightmost_x - leftmost_x;
      int center_x = leftmost_x + width/2;
      int upmost_y = max(y0, max(y1, max(y2, y3)));
      int downmost_y = min(y0, min(y1, min(y2, y3)));
      int height = upmost_y - downmost_y;
      int center_y = downmost_y + height/2;

      pin* center_pin = new pin(name, center_x, center_y);
      HardBlock *HB = new HardBlock(name, width, height, center_pin, x0, y0);
      HBList.emplace_back(HB);
      HBTable[name] = HB;
      PinTable[name] = HB->center_pin;
    }
  }
}

void ArgParser::ReadPin(string const & filepath)
{
  ifstream fin_pl(filepath);
  string pin_name;
  int x_cor, y_cor;
  while(fin_pl >> pin_name >> x_cor >> y_cor)
  {
    pin *cur_pin = new pin(pin_name, x_cor, y_cor);
    PinTable[pin_name] = cur_pin;
  }
}

void ArgParser::ReadNet(string const & filepath)
{
  ifstream fin_nets(filepath);
  string tmp;
  while(getline(fin_nets, tmp))
  {
    if(tmp[3] != 'D') continue;
    stringstream ss(tmp);
    string temp, colon;
    int degree;
    while(ss >> temp >> colon >> degree)
    {
      net *cur_net = new net(degree);
      NetList.emplace_back(cur_net);
      for(int i = 0; i < degree; ++i)
      {
        string terminal;
        fin_nets >> terminal;
        if(terminal[0] == 'p')
        { 
          auto fixed_pin = PinTable[terminal];
          NetList.back()->pins.emplace_back(fixed_pin);
        }
        else
        {
          auto hb_pin = HBTable[terminal];
          NetList.back()->hardblocks.emplace_back(hb_pin);
        }
      }
    }
  }
}

Input* ArgParser::ReadFile(char* argv[])
{
  // int seed = stoi(argv[6]);
  double dead_space_ratio = stod(argv[5]);
  ReadHardBlock(argv[1]);
  ReadPin(argv[3]);
  ReadNet(argv[2]);
  // Input* input = new Input(HBList, HBTable, NetList, dead_space_ratio, seed);
  Input* input = new Input(HBList, HBTable, NetList, dead_space_ratio);
  return input;
}