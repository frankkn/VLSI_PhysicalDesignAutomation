#include <iostream>
#include "./Module.h"
using namespace std;

void SubRow::updateInfo(int const &new_x_min, int const &new_x_max)
{
  x_min = new_x_min;
  x_max = new_x_max;
  capacity = x_max - x_min;
}