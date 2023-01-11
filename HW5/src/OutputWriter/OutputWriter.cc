#include <fstream>
#include <iostream>
#include "OutputWriter.h"
using namespace std;

void OutputWriter::WriteResult(char *argv[])
{
  ofstream out(argv[2]);
  out << "VERSION 5.6 ;\n";
  out << "DIVIDERCHAR \"/\" ;\n";
  out << "BUSBITCHARS \"[]\" ;\n";
  
  out << "DESIGN " << input->die->design_name << " ;\n\n";
  out << "UNITS DISTANCE MICRONS 1000 ;\n\n";
  out << "PROPERTYDEFINITIONS\n";
  out << "  COMPONENTPIN text STRING ;\n";
  out << "END PROPERTYDEFINITIONS\n\n";
  out << "DIEAREA ( " << input->die->x1 << " " << input->die->y1 << " ) ( " << input->die->x2 << " " << input->die->y2 << " ) ;\n\n";

  out << "COMPONENTS " << 10*input->numCS << " ;\n";
  for(auto row = input->cs_array.begin(); row != input->cs_array.end(); row++)
  {
    for(auto col = row->begin(); col != row->end(); col++)
    {
      out << "- " << (*col)->inst_name << " " << (*col)->lib_name << "\n";
      out << "  + PLACED ( " << (*col)->x << " " << (*col)->y << " ) N ;\n";
    }
  }

  for(auto row = input->Via34_port2ME3.begin(); row != input->Via34_port2ME3.end(); row++)
  {
    for(auto col = row->begin(); col != row->end(); col++)
    {
      out << "- " << (*col)->inst_name << " " << (*col)->lib_name << "\n";
      out << "  + PLACED ( " << (*col)->x << " " << (*col)->y << " ) N ;\n";
    }
  }

  for(auto row = input->Via34_drain2ME3.begin(); row != input->Via34_drain2ME3.end(); row++)
  {
    for(auto col = row->begin(); col != row->end(); col++)
    {
      out << "- " << (*col)->inst_name << " " << (*col)->lib_name << "\n";
      out << "  + PLACED ( " << (*col)->x << " " << (*col)->y << " ) N ;\n";
    }
  }
  out << "END COMPONENTS\n\n";

  out << "SPECIALNETS " << 7*input->numCS << " ;\n";
  for(auto row = input->ME3_specialnet.begin(); row != input->ME3_specialnet.end(); row++)
  {
    for(auto col = row->begin(); col != row->end(); col++)
    {
      out << "- " << (*col)->inst_name << "\n";
      out << "  + ROUTED ME3 440 ( " << ((*col)->x1 + (*col)->x2) /2 << " " << (*col)->y1 << " ) ( * " << (*col)->y2 << " ) ;\n";
    }
  }

  for(auto row = input->ME4_specialnet_drain.begin(); row != input->ME4_specialnet_drain.end(); row++)
  {
    for(auto col = row->begin(); col != row->end(); col++)
    {
      out << "- " << (*col)->inst_name << "\n";
      out << "  + ROUTED ME4 1000 ( " << (*col)->x1 << " " << ((*col)->y1 + (*col)->y2) /2 << " ) ( " << (*col)->x2 << " * ) ;\n";
    }
  }

  for(auto row = input->ME4_specialnet_port.begin(); row != input->ME4_specialnet_port.end(); row++)
  {
    for(auto col = row->begin(); col != row->end(); col++)
    {
      out << "- " << (*col)->inst_name << "\n";
      out << "  + ROUTED ME4 1000 ( " << (*col)->x1 << " " << ((*col)->y1 + (*col)->y2) /2 << " ) ( " << (*col)->x2 << " * ) ;\n";
    }
  }
  out << "END SPECIALNETS\n\n";

  out << "END DESIGN\n";
}
