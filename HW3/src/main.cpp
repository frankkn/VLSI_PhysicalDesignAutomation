#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <stack>
using namespace std;

/* 
./main.out ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100.out 0.1
*/

struct pin
{
  string id; 
  int x_cor, y_cor;

  pin(string id, int x_cor, int y_cor):id(id), x_cor(x_cor), y_cor(y_cor) {}
};
unordered_map<string, pin*> PinTable;

struct HardBlock
{
  string name;
  int width, height;
  int downleft_x, downleft_y; 
  bool rotated;
  pin* center_pin;

  HardBlock(string name, int width, int height, int downleft_x = 0, int downleft_y = 0):
    name(name), width(width), height(height), rotated(false), downleft_x(downleft_x), downleft_y(downleft_y), center_pin(new pin(name, downleft_x, downleft_y)) {}
};
vector<HardBlock*> HBList;
unordered_map<string, HardBlock*> HBTable;

struct net
{
  int degree;
  vector<pin*> pins;
  vector<HardBlock*> hardblocks;

  net(int degree = 0):degree(degree) {}

  int HPWL();
};
vector<net*> NetList;

struct TreeNode
{
    int type; //0:hb, -1:Vertical, -2:Horizontal
    int width, weight;
    HardBlock* hardblock;
    TreeNode *lchild, *rchild;
    vector<tuple<int, int, pair<int,int>>> shape; // For VHnode (cut node)
    
    TreeNode(int type, HardBlock* hardblock = nullptr):
      type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr)
    {
      if(type == 0) // leaf block shape
      {
        shape.emplace_back(make_tuple(hardblock->width, hardblock->height, (0,0)));
        shape.emplace_back(make_tuple(hardblock->height, hardblock->width, (0,0)));
      }
    }
    void updateShape();
};

void TreeNode::updateShape()
{

}

TreeNode* ConstructTree(vector<int>& NPE)
{
  stack<TreeNode*> st;
  for(auto& element:NPE)
  {
    if(element >= 0)
    {
      string hbNode_name = "sb"+element;
      HardBlock* hb = HBTable[hbNode_name];
      TreeNode* hbNode = new TreeNode(0, hb);
      st.emplace(hbNode);
    }
    else
    {
      TreeNode* VHnode = new TreeNode(element);
      TreeNode* Rnode = st.top(); st.pop();
      VHnode->rchild = Rnode;
      TreeNode* Lnode = st.top(); st.pop();
      VHnode->lchild = Lnode;
      st.emplace(VHnode);
      VHnode->updateShape();
    }
  }
  return st.top(); // root
}

// int calHPWL()
// {

// }

int main(int argc, char *argv[])
{
  cin.tie(nullptr);
  ios::sync_with_stdio(false);

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

      HardBlock *HB = new HardBlock(name, width, height, x0, y0);
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

  // Step 2-1: Calculate total area to determine the width and height of floorplan region
  int total_area = 0;
  for(const auto& hb: HBList)
  {
    int w = hb->width, h = hb->height;
    total_area += (w * h);
  }
  double dead_space_ratio = stod(argv[5]);
  int region_side_len = sqrt(total_area * (1+dead_space_ratio));

  // Step 2-2: Init Normalized Polish Expression
  vector<int> NPE;
  int cur_width = 0;
  NPE.emplace_back(0);
  for(int i = 1; i < HBList.size(); ++i)
  {
    auto curHB = HBList[i];
    NPE.emplace_back(i);
    if(cur_width + curHB->width > region_side_len)
    {
      NPE.emplace_back(-2);
      cur_width = curHB->width;
    }
    else
    {
      cur_width += curHB->width;
      NPE.emplace_back(-1);
    }
  }

  // Step 3: Simulated Annealing Floorplanning
  vector<int> BestNPE = NPE;
  double T0 = 100;
  int M = 0, MT = 0, uphill = 0;
  int k = 5, N = k * HBList.size();

  return 0;
}
