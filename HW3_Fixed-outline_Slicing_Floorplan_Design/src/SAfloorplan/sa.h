#pragma once
#include <vector>
#include "../Module/module.h"
#include "../Clock/clock.h"
using namespace std;

class SA
{ 
  private:
    double region_side_len;

    Clock& clock;

    void CalSideLen(double& dead_space_ratio);
    template<class T> void SWAP(T& a, T& b); // "perfect swap" (almost)
    void InitNPE(vector<int>& NPE);
    void Complement(vector<int>& curNPE, int startIdx);
    bool isSkewed(vector<int>& curNPE, int i);
    bool violateBallot(vector<int>& curNPE, int i);
    vector<int> Perturb(vector<int> curNPE, int M);
    TreeNode* ConstructTree(vector<int>& NPE);
    void PlaceBlock(TreeNode* node, int shapeIdx, int new_x, int new_y);
    int CalTotalHPWL();
    int CalCost(vector<int>& NPE, bool const & forWL);
    void SAfloorplanning(double epsilon, double r, int k, bool forWL, vector<int>& curNPE, vector<int>& bestNPE);
  public:
    SA(double dead_space_ratio, Clock& clock):clock(clock) { CalSideLen(dead_space_ratio); }
    int Run();
};