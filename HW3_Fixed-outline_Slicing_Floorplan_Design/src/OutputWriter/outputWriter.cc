#include <fstream>
#include "outputWriter.h"
using namespace std;

void OutputWriter::WriteResult(string const & filepath)
{
  ofstream output(filepath);
  int WL = 0;
  for(auto& net: input->NetList)
  {
    WL += net->calHPWL();
  }
  output << "Wirelength " << WL << "\n";
  output << "Blocks" << "\n";
  for(auto& hb: input->HBList)
  {
    output << hb->name << " " << hb->downleft_x << " " << hb->downleft_y << " " << hb->rotated << "\n"; 
  }
}