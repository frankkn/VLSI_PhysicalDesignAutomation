#include <iostream>
#include <string>
#include "Module/Module.h"
#include "Placer/Placer.h"
#include "OutputWriter/OutputWriter.h"
using namespace std;

int main(int argc, char **argv)
{
  auto input = new Input(argv);
  
  Placer placer(input);
  OutputWriter* OW = placer.run();
  // OW->WriteResult(argv);
	return 0;
}
