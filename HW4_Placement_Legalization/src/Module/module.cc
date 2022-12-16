#include <iostream>
#include "./module.h"
using namespace std;

void SubRow::updateInfo(int &newLeftX, int &newRightX)
{
  leftX = newLeftX;
  rightX = newRightX;
  capacity = newRightX - newLeftX;
}