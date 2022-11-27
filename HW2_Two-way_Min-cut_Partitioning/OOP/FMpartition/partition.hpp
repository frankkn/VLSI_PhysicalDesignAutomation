#pragma once
#include "../ArgParser/argparser.cpp"
#include "../Module/module.cpp"

class FM
{
  private:
    Input* input;
    BucketList bucketList;
    vector<Set> sets;
    
  public:
    FM (Input* input):input(input) {}
    void InitPartition();
    void calNetGroupCnt();
    int calCutSize();
    void InitGain();
};