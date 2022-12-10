#pragma once
#include "../Module/module.h"
using namespace std;

class AbacusLegalizer {
  private:
    Input* input;
    
    // void cutSubRow();

  public:
    AbacusLegalizer(Input* input):input(input) {}
    void cutSubRow();
    // OutputWriter* run();
};