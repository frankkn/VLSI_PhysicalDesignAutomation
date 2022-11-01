#pragma once
#include "../Module/module.cpp"

class Input
{
  public:
    // int setAsize;
    vector<Cell*> cellArray;
    vector<Net*> netArray;

    Input(vector<Cell*> cellArray, vector<Net*> netArray):cellArray(cellArray), netArray(netArray) {}
};

class ArgParser
{
  private:
    // int setAsize;
    vector<Cell*> cellArray;
    vector<Net*> netArray;

  public:
    ArgParser() {}
    Input* parse(char *argv[]);

};