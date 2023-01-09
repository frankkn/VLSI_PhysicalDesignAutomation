#pragma once
#include "../Module/Module.h"
#include "../OutputWriter/OutputWriter.h"

class Placer
{
  Input *input;
  void createDieBoundary();
  void createCSPlacement();
  void createVerticalME3();
  void createME4Drain();
  void createME4Port();
  // void createVia34_drain2ME3();
  // void createVia34_port2ME3();

  public:
    Placer(Input *input): input(input) {}
    OutputWriter *run();
};