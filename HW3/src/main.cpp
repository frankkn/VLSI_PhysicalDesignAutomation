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
#include <climits>
#include <stdlib.h>
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

  int calHPWL();
};
vector<net*> NetList;

int net::calHPWL()
{
  int min_x = INT_MAX, max_x = INT_MIN;
  int min_y = INT_MAX, max_y = INT_MIN;
  for(const auto& pin: pins)
  {
    if(pin->x_cor < min_x)  min_x = pin->x_cor;
    if(pin->x_cor > max_x)  max_x = pin->x_cor;
    if(pin->y_cor < min_y)  min_y = pin->y_cor;
    if(pin->y_cor > max_y)  max_y = pin->y_cor;
  }
  for(const auto& hb: hardblocks)
  {
    if(hb->center_pin->x_cor < min_x) min_x = hb->center_pin->x_cor;
    if(hb->center_pin->x_cor > max_x) max_x = hb->center_pin->x_cor;
    if(hb->center_pin->y_cor < min_y) min_y = hb->center_pin->y_cor;
    if(hb->center_pin->y_cor > max_y) max_y = hb->center_pin->y_cor;
  }
  return max_x - min_x + max_y - min_y;
}

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
      // if(type == 0) // leaf block shape
      // {
      //   shape.emplace_back(make_tuple(hardblock->width, hardblock->height, (0,0)));
      //   shape.emplace_back(make_tuple(hardblock->height, hardblock->width, (0,0)));
      // }
    }
    void updateShape();
};

/*
void TreeNode::updateShape()
{
  decltype(shape)().swap(shape);
  if(type == -1)
  {
    sort(lchild->shape.begin(), lchild->shape.end(), [&](tuple<int,int,pair<int,int>>& a, tuple<int,int,pair<int,int>>& b){ return get<1>(a) > get<1>(b); });
    sort(rchild->shape.begin(), rchild->shape.end(), [&](tuple<int,int,pair<int,int>>& a, tuple<int,int,pair<int,int>>& b){ return get<1>(a) > get<1>(b); });
    int i = 0, j = 0;
    while(i < lchild->shape.size() && j < rchild->shape.size())
    {
      auto cur_shape = make_tuple(get<0>(lchild->shape[i])+get<0>(rchild->shape[j]),max(get<1>(lchild->shape[i]),get<1>(rchild->shape[j])),(i,j));
      shape.emplace_back(cur_shape);
      if(get<1>(lchild->shape[i]) > get<1>(rchild->shape[j]))
      {
        ++i;
      }
      else if(get<1>(lchild->shape[i]) < get<1>(rchild->shape[j]))
      {
        ++j;
      }
      else
      {
        ++i;++j;
      }
    }
  }
  else
  {
    sort(lchild->shape.begin(), lchild->shape.end(), [&](tuple<int,int,pair<int,int>>& a, tuple<int,int,pair<int,int>>& b){ return get<0>(a) > get<0>(b); });
    sort(rchild->shape.begin(), rchild->shape.end(), [&](tuple<int,int,pair<int,int>>& a, tuple<int,int,pair<int,int>>& b){ return get<0>(a) > get<0>(b); });
    int i = 0, j = 0;
    while(i < lchild->shape.size() && j < rchild->shape.size())
    {
      auto cur_shape = make_tuple(max(get<0>(lchild->shape[i]),get<0>(rchild->shape[j])),get<1>(lchild->shape[i])+get<1>(rchild->shape[j]),(i,j));
      shape.emplace_back(cur_shape);
      if(get<0>(lchild->shape[i]) > get<0>(rchild->shape[j]))
      {
        ++i;
      }
      else if(get<0>(lchild->shape[i]) < get<0>(rchild->shape[j]))
      {
        ++j;
      }
      else
      {
        ++i;++j;
      }
    }
  }
}
*/
/*
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
*/

// template<class T>
// void SWAP(T& a, T& b) // "perfect swap" (almost)
// {
//   T tmp {move(a)}; // move from a
//   a = move(b); // move from b
//   b = move(tmp); // move from tmp
// }

/*
vector<int>& selectMove(vector<int>& curNPE, int M)
{
  vector<int> LeafPos;
  switch(M)
  {
    case 0:
      for(int i = 0; i < curNPE.size(); ++i)
      {
        if(curNPE[i] >= 0)
        {
          LeafPos.emplace_back(i);
        }
      }
      int n = LeafPos.size();
      int firstIdx = rand() % n, secondIdx = rand() % n;
      while(firstIdx == secondIdx)  firstIdx = rand() % n;
      SWAP(curNPE[firstIdx], curNPE[secondIdx]);
      break;
    case 1:

      break;
    case 2:  

      break;
  }
  return curNPE;
}
*/

// vector<int> SAfloorplanning(int epsilon, int ratio, int k, vector<int>& NPE)
// {
//   vector<int> BestNPE {}, curNPE = NPE;
//   double T0 = 100;
//   int MT = 0, uphill = 0, reject = 0;
//   int N = k * HBList.size();
//   while(reject/MT <= 0.95 && T0 >= epsilon)
//   {
//     int M = rand() % 3;
//     vector<int> curNPE = selectMove(curNPE, M);
//   }
// }

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
  vector<int> NPE, row;
  deque<vector<int>> rows;
  int cur_width = HBList[0]->width;
  row.emplace_back(0);
  //int placedBlock = 1;
  for(int i = 1; i < HBList.size(); ++i)
  {
    auto curHB = HBList[i];
    if(cur_width + curHB->width <= region_side_len)
    {
      row.emplace_back(i);
      cur_width += curHB->width;
      row.emplace_back(-1);
    }
    else
    {
      rows.emplace_back(row);
      decltype(row)().swap(row);

      row.emplace_back(i);
      cur_width = curHB->width;
    }
  }
  rows.emplace_back(row);

  for(auto& row_i: rows[0])
  {
    NPE.emplace_back(row_i);
  }
  rows.pop_front();

  for(auto& row:rows)
  {
    for(auto& row_i:row)
    {
      NPE.emplace_back(row_i);
    }
    NPE.emplace_back(-2);
  }

  cout << NPE.size() << "!\n";
  for(auto NPE_i: NPE)  cout << NPE_i << " ";

  // Step 3: Simulated Annealing Floorplanning
  int epsilon = 0.5; // End Temperature
  int ratio = 0.95; // Decreasing ratio for temperature
  int k = 5; 
  // SAfloorplanning(epsilon, ratio, k, NPE);

  return 0;
}
