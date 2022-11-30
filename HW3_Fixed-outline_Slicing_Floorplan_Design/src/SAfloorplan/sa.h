#pragma once
#include <vector>
#include "../Module/module.h"
#include "../Clock/clock.h"
#include "../OutputWriter/outputWriter.h"
using namespace std;

class SA
{ 
  private:
    double region_side_len;
    Input* input;
    Clock& clock;
    vector<TreeNode*> hbNodeList, cutNodeList;

    void CalSideLen(double& dead_space_ratio);
    template<class T> void SWAP(T& a, T& b); // "perfect swap" (almost)
    void InitNPE(vector<int>& NPE);
    void Complement(vector<int>& curNPE, int startIdx);
    bool isSkewed(vector<int>& curNPE, int i);
    bool violateBallot(vector<int>& curNPE, int i);
    vector<int>& Perturb(vector<int>& curNPE, int M);
    TreeNode* ConstructTree(vector<int>& NPE);
    void PlaceBlock(TreeNode* node, int shapeIdx, int new_x, int new_y);
    int CalTotalHPWL();
    int CalSACost(vector<int>& NPE, bool const & forArea);
    void SAfloorplanning(double epsilon, double r, int k, bool forArea, vector<int>& curNPE, vector<int>& bestNPE);
  public:
    SA(Input* input, Clock& clock):input(input), clock(clock)
    { 
      CalSideLen(input->dead_space_ratio); 
      for(int i = 0; i < input->HBList.size(); ++i)
      {
        HardBlock* cur_hb = input->HBList[i];
        TreeNode* hbNode = new TreeNode(0, cur_hb);
        hbNodeList.emplace_back(hbNode);
      }
      for(int i = 0; i < input->HBList.size()-1; ++i)
      {
        TreeNode* cutNode = new TreeNode(-1);
        cutNodeList.emplace_back(cutNode);
      }
    }
    OutputWriter* Run();
};