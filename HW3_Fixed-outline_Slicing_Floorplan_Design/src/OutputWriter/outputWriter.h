#pragma once
#include "../Module/module.h"
using namespace std;

class OutputWriter
{
  private:
    Input* input;

  public:
    OutputWriter(Input* input):input(input) {}
    void WriteResult(string const & filepath);
};