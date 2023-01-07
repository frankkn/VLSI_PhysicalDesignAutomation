#pragma once
#include <string>
#include <vector>
using namespace std;

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

  Die() {}
  Die(string const &design_name, int const &x1, int const &y1, int const &x2, int const &y2)
    :design_name(design_name), x1(x1), y1(y1), x2(x2), y2(y2) {}
};

struct Component
{
  string lib_name, inst_name;
  int x, y;

  inline void setPos(int const &_x, int const &_y)
  {
    this->x = _x;
    this->y = _y;
  }

  Component(string const &lib_name, string const &inst_name)
    :lib_name(lib_name), inst_name(inst_name), x(x), y(y) {}
};

struct SpecialNet
{
  string inst_name, layer;
  int x1, y1, x2, y2;

  inline void setPos(int const &_x1, int const &_y1, int const &_x2, int const &_y2)
  {
    this->x1 = _x1;
    this->y1 = _y1;
    this->x2 = _x2;
    this->y2 = _y2;
  }

  SpecialNet(string const &inst_name, string const &layer, int const &x1, int const &y1, int const &x2, int const &y2)
    :inst_name(inst_name), layer(layer), x1(x1), y1(y1), x2(x2), y2(y2) {}
};

struct GlobalParameter
{
  int CS_WIDTH = 7100;
  int CS_HEIGHT = 6600;
  int M3_WIDTH = 440;
  int M3_SPACING = 310;
  int M4_WIDTH = 1000;
  int M4_SPACING = 490;

  int CS_X1_TO_DRAIN = 1260;
  int CS_Y1_TO_DRAIN = 4100;
  string CS_LIB_NAME = "MSBCS";
  string VIA34_LIB_NAME = "Via34";

  GlobalParameter() {}
};

struct Input
{
	int numCS;
  Die die;
  GlobalParameter GP;
  vector<Component> cs_array, Via34_drain2ME3, Via34_port2ME3;
  vector<SpecialNet> ME3_specialnet, ME4_specialnet_drain, ME4_specialnet_port;

	Input(char **argv):numCS(stoi(argv[1])) {}
};