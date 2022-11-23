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
#include <time.h>
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
  void updateInfo(int& new_width, int& new_height, int& new_x, int& new_y);

  HardBlock(string name, int width, int height, int downleft_x = 0, int downleft_y = 0):
    name(name), width(width), height(height), rotated(false), downleft_x(downleft_x), downleft_y(downleft_y), center_pin(new pin(name, downleft_x, downleft_y)) {}
};
vector<HardBlock*> HBList;
unordered_map<string, HardBlock*> HBTable;

void HardBlock::updateInfo(int& new_width, int& new_height, int& new_x, int& new_y)
{
  downleft_x = new_x;
  downleft_y = new_y;
  rotated = width == new_width? rotated:!rotated;
  center_pin->x_cor = new_x + new_width/2;
  center_pin->y_cor = new_y + new_height/2;
}

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
    vector<tuple<int,int,pair<int,int>>> shape; // For VHnode (cut node)
    
    TreeNode(int type = 0, HardBlock* hardblock = nullptr):
      type(type), hardblock(hardblock), lchild(nullptr), rchild(nullptr)
    {
      if(type == 0) // leaf block shape
      {
        shape.emplace_back(make_tuple(hardblock->width, hardblock->height, make_pair(0,0)));
        shape.emplace_back(make_tuple(hardblock->height, hardblock->width, make_pair(0,0)));
      }
    }
    void updateShape();
};

void TreeNode::updateShape()
{
  decltype(shape)().swap(shape);
  if(type == -1)
  {
    sort(lchild->shape.begin(), lchild->shape.end(), [&](auto& a, auto& b) {return get<1>(a) > get<1>(b);});
    sort(rchild->shape.begin(), rchild->shape.end(), [&](auto& a, auto& b) {return get<1>(a) > get<1>(b);});
    int i = 0, j = 0;
    while(i < lchild->shape.size() && j < rchild->shape.size())
    {
      auto cur_shape = make_tuple(get<0>(lchild->shape[i])+get<0>(rchild->shape[j]), max(get<1>(lchild->shape[i]), get<1>(rchild->shape[j])),make_pair(i,j));
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
    sort(lchild->shape.begin(), lchild->shape.end(), [&](auto& a, auto& b) {return get<0>(a) > get<0>(b);});
    sort(rchild->shape.begin(), rchild->shape.end(), [&](auto& a, auto& b) {return get<0>(a) > get<0>(b);});
    int i = 0, j = 0;
    while(i < lchild->shape.size() && j < rchild->shape.size())
    {
      auto cur_shape = make_tuple(max(get<0>(lchild->shape[i]),get<0>(rchild->shape[j])), get<1>(lchild->shape[i])+get<1>(rchild->shape[j]), make_pair(i,j));
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

class SA
{ 
  private:
    double region_side_len;
    void CalSideLen(double& dead_space_ratio);
    template<class T> void SWAP(T& a, T& b); // "perfect swap" (almost)
    void InitNPE(vector<int>& NPE);
    void Complement(vector<int>& curNPE, int startIdx);
    bool isSkewed(vector<int>& curNPE, int i);
    bool isBallot(vector<int>& curNPE, int i);
    vector<int>& selectMove(vector<int>& curNPE, int M);
    TreeNode* ConstructTree(vector<int>& NPE);
    void PlaceBlock(TreeNode* node, int shapeIdx, int new_x, int new_y);
  public:
    SA(double dead_space_ratio) { CalSideLen(dead_space_ratio); }
    void Run();
};

void SA::CalSideLen(double& dead_space_ratio)
{
  double total_area = 0;
  for(const auto& hb: HBList)
  {
    int w = hb->width, h = hb->height;
    total_area += (w * h);
  }
  region_side_len = sqrt(total_area * (1+dead_space_ratio));
}

template<class T>
void SA::SWAP(T& a, T& b) // "perfect swap" (almost)
{
  T tmp {move(a)}; // move from a
  a = move(b); // move from b
  b = move(tmp); // move from tmp
}

void SA::InitNPE(vector<int>& NPE)
{
  vector<int> row;
  deque<vector<int>> rows;
  int cur_width = HBList[0]->width;
  row.emplace_back(0);
  int placedBlock = 1;
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
  //NPE.emplace_back(-2);

  for(auto& row:rows)
  {
    for(auto& row_i:row)
    {
      NPE.emplace_back(row_i);
    }
    NPE.emplace_back(-2);
  }
}

void SA::Complement(vector<int>& curNPE, int startIdx)
{
  for(int i = startIdx; i < curNPE.size(); ++i)
  {
    if(curNPE[i] == -1) curNPE[i] = -2;
    else if(curNPE[i] == -2)  curNPE[i] = -1;
    else break;
  }
}

bool SA::isSkewed(vector<int>& curNPE, int i)
{
  // SWAP(curNPE[i], curNPE[i+1]);
  // if(curNPE[i-1] == curNPE[i])  return false;
  // else if(curNPE[i] == curNPE[i+1]) return false;
  // return true;

  if(i-1 >= 0 && curNPE[i-1] == curNPE[i+1])
  {
    return false;
  }
  else if(i+2 < curNPE.size() && curNPE[i] == curNPE[i+2])
  {
    return false;
  }
  return true;
}

bool SA::isBallot(vector<int>& curNPE, int i)
{
  // SWAP(curNPE[i], curNPE[i+1]);
  // int N = 0;
  // for(int k = 0; k <= i+1; ++k)
  // {
  //   if(curNPE[i] == -1 || curNPE[i] == -2)  ++N;
  //   if(2 * N < i) return false;
  // }
  // return true;

  // operand + operator
  if(curNPE[i] >= 0)
  {
    int N = 0;
    for(int k = 0; k <= i+1; ++k)
    {
      if(curNPE[i] == -1 || curNPE[i] == -2)  ++N;
      if(2 * N < i) return false;
    }
  }
  return true;
}

vector<int>& SA::selectMove(vector<int>& curNPE, int M)
{
  unsigned seed = (unsigned)time(NULL);
  srand(seed);
  switch(M)
  {
    case 0:
    {
      vector<int> SwapPos;
      for(int i = 0; i < curNPE.size(); ++i)
      {
        if(curNPE[i] >= 0 && SwapPos.size() < HBList.size()-1)
        {
          SwapPos.emplace_back(i);
        }
      }
      int n = SwapPos.size();
    
      int r = rand() % n;
      int pos1 = SwapPos[r];
      int pos2 = pos1;
      while(curNPE[++pos2] < 0);
      // cout << "pos1 = " << pos1 << " Block num = " << curNPE[pos1] << endl;
      // cout << "pos2 = " << pos2 << " Block num = " << curNPE[pos2] << endl;
      SWAP(curNPE[pos1], curNPE[pos2]);
      break;
    }
    case 1:
    {
      vector<int> InverseStartPos;
      for(int i = 0; i < curNPE.size()-1; ++i)
      {
        if(curNPE[i] >= 0 && curNPE[i+1] < 0)
        {
          InverseStartPos.emplace_back(i+1);
        }
      }
      int n = InverseStartPos.size();
      int r = rand() % n;
      int startIdx = InverseStartPos[r];
      // cout << "StartIdx of curNPE = " << startIdx << endl;
      Complement(curNPE, startIdx);
      break;
    }
    case 2:  
      vector<int> SwapPos;
      for(int i = 0; i < curNPE.size()-1; ++i)
      {
        if(curNPE[i] >= 0 && curNPE[i+1] < 0)
        {
          if(isSkewed(curNPE, i) && isBallot(curNPE, i))
          {
            SwapPos.emplace_back(i);
          }
        }
        else if(curNPE[i] < 0 && curNPE[i+1] >= 0)
        {
          if(isSkewed(curNPE, i))
          {
            SwapPos.emplace_back(i);
          }
        }
      }
      int n = SwapPos.size();
      if(n != 0)
      {
        int r = rand() % n;
        int SwapIdx = SwapPos[r];
        // cout << "SwapIdx = " << SwapIdx << endl;
        SWAP(curNPE[SwapIdx], curNPE[SwapIdx+1]);
      }
      break;
  }
  return curNPE;
}

TreeNode* SA::ConstructTree(vector<int>& NPE)
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

void SA::PlaceBlock(TreeNode* node, int shapeIdx, int x, int y)
{
  if(node->type == 0)
  {
    node->hardblock->updateInfo(get<0>(node->shape[shapeIdx]), get<1>(node->shape[shapeIdx]), x, y);
  }
  else
  {
    PlaceBlock(node->lchild, get<2>(node->shape[shapeIdx]).first, x, y);
    int new_x = 0, new_y = 0;
    new_x += node->type == -1? get<0>(node->lchild->shape[get<2>(node->shape[shapeIdx]).first]): 0;
    new_y += node->type == -1? 0: get<1>(node->lchild->shape[get<2>(node->shape[shapeIdx]).first]);
    PlaceBlock(node->rchild, get<2>(node->shape[shapeIdx]).second, new_x, new_y);
  }
}

// First row and second row will be concatenated.
// Second row will start with two continuous hardblocks(>= 0).
void PrintInit(vector<int>& NPE)
{
  vector<vector<int>> rows;
  vector<int> row;
  for(auto NPE_i: NPE)
  {
    row.emplace_back(NPE_i);
    if(NPE_i == -2)
    { 
      rows.emplace_back(row);
      row.clear();
    }
  }
  reverse(rows.begin(), rows.end());
  for(auto row: rows)
  {
    for(auto row_i: row)
    {
      cout << row_i << " ";
      if(row_i == -2) cout << "\n";
    }
  }
}

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

  SA sa(stod(argv[5]));

  //sa.Run();
  //WriteResult(argv[4], HBList);

  return 0;
}
