#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <algorithm>
using namespace std;

struct pin
{
  string id; 
  int x_cor, y_cor;

  pin(string id, int x_cor, int y_cor):id(id), x_cor(x_cor), y_cor(y_cor) {}
};

struct HardBlock
{
  string name;
  int width, height, center_x, center_y;
  bool rotated;
  pin* center_pin;

  HardBlock(string name, int width, int height, int center_x = 0, int center_y = 0):
    name(name), width(width), height(height), rotated(false), center_x(center_x), center_y(center_y), center_pin(new pin(name, center_x, center_y)) {}
};

vector<HardBlock*> HBTable;

struct net
{
  int degree;
  vector<pin*> pins;
  vector<HardBlock*> hardblocks;

  int HPWL();
};

struct TreeNode
{
    int type, width, weight;
    HardBlock *hardblock;
    TreeNode *lchild, *rchild;
    vector<tuple<int, int, pair<int,int>>> shape;
    
    TreeNode(int type, HardBlock* hardblock = nullptr):
      type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr)
    {
      if(hardblock != nullptr)
      {
        shape.emplace_back(make_tuple(hardblock->width, hardblock->height, make_pair(0,0)));
        shape.emplace_back(make_tuple(hardblock->height, hardblock->width, make_pair(1,1)));
      }
    }
    void updateShape();
};

int main(int argc, char *argv[])
{
  cin.tie(nullptr);
  ios::sync_with_stdio(false);

  // Step 1: Read hard blocks
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
      int width = leftmost_x - leftmost_x;
      int center_x = leftmost_x + width/2;
      int upmost_y = max(y0, max(y1, max(y2, y3)));
      int downmost_y = min(y0, min(y1, min(y2, y3)));
      int height = upmost_y - downmost_y;
      int center_y = downmost_y + height/2;

      HardBlock *HB = new HardBlock(name, width, height, center_x, center_y);
      HBTable.emplace_back(HB);
    }
  }
  
  for(auto& HB:HBTable)
  {
    cout << HB->name << "\n";
  }

  return 0;
}
