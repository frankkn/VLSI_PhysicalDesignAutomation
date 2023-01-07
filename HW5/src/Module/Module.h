#pragma once
#include <vector>

struct Die
{
  string design_name;
  int x1, y1, x2, y2;

  inline void setPos(int const &_x1, int const &_y1, int const &_x2, int const &_y2)
  {
    this->x1 = _x1;
    this->y1 = _y1;
    this->x2 = _x2;
    this->y2 = _y2;
  }

  Die(string const &design_name, int const &x1, int const &y1, int const &x2, int const &y2)
    :design_name(design_name), x1(x1), y1(y1), x2(x2), y2(y2) {}
};

struct Component
{
  string lib_name, inst_name;
  int x, y;

  Component(string const &lib_name, string const &inst_name)
    :lib_name(lib_name), inst_name(inst_name), x(x), y(y) {}
};

struct SpecialNet
{
  string inst_name, layer;
  int x1, y1, x2, y2;

  SpecialNet(string const &inst_name, string const &layer, int const &x1, int const &y1, int const &x2, int const &y2)
    :inst_name(inst_name), layer(layer), x1(x1), y1(y1), x2(x2), y2(y2) {}
};

struct Input
{
	int numCS;
  Die die;
  vector<Component> cs_array, Via34_drain2ME3, Via34_port2ME3;
  vector<SpecialNet> ME3_specialnet, ME4_specialnet_drain, ME4_specialnet_port;

	Input(char **argv):numCS(stoi(argv[1])) {}
};