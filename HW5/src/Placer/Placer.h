#pragma once
#include "../Module/Module.h"
#include "../OutputWriter/OutputWriter.h"

class Placer
{
  Input* input;

  public:
    Placer(Input* input): input(input) {}
    OutputWriter* run();
};