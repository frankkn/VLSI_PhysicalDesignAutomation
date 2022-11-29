#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "./Module/module.h"
#include "./SAfloorplan/sa.h"
#include "./Clock/clock.h"

using namespace std;

unordered_map<string, pin*> PinTable;
vector<HardBlock*> HBList;
unordered_map<string, HardBlock*> HBTable;
vector<net*> NetList;

void WriteResult(string filename, int WL)
{
  ofstream output(filename);

  output << "Wirelength " << WL << "\n";
  output << "Blocks" << "\n";
  for(auto& hb:HBList)
  {
    output << hb->name << " " << hb->downleft_x << " " << hb->downleft_y << " " << hb->rotated << "\n"; 
  }
}

int main(int argc, char *argv[])
{
  Clock clock(10*60 - 6);
  clock.StartClock("Total time");
  clock.StartClock("Input time");

  // Step 1-1: Read hard blocks
  ifstream fin_hardblocks(argv[1]);
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
  
  // Step 1-2: Read pin
  ifstream fin_pl(argv[3]);
  string pin_name;
  int x_cor, y_cor;
  while(fin_pl >> pin_name >> x_cor >> y_cor)
  {
    pin *cur_pin = new pin(pin_name, x_cor, y_cor);
    PinTable[pin_name] = cur_pin;
  }

  // Step 1-3: Read nets
  ifstream fin_nets(argv[2]);
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
  clock.EndClock("Input time");
  
  clock.StartClock("SA time");
  SA sa(stod(argv[5]), clock);
  int finalWL = sa.Run();
  clock.EndClock("SA time");

  clock.StartClock("Output time");
  WriteResult(argv[4], finalWL);
  clock.EndClock("Output time");

  clock.EndClock("Total time");

  clock.PrintDuration("Input time");
  clock.PrintDuration("SA time");
  clock.PrintDuration("Output time");
  clock.PrintDuration("Total time");

  return 0;
}