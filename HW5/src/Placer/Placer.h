#pragma once
#include "../Module/Module.h"
#include "../OutputWriter/OutputWriter.h"

class Placer
{
  Input *input;
  void createDieBoundary();
  void createCSPlacement();

  public:
    Placer(Input *input): input(input) {}
    OutputWriter *run();
};