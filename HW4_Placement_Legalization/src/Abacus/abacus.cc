#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include "./abacus.h"
using namespace std;

void AbacusLegalizer::cutSubRow() 
{
  sort(input->terminals.begin(), input->terminals.end(), [&](auto &a, auto &b){ return a->x < b->x; });
  for(auto &corerow: input->block) 
  {
    for(auto &terminal: input->terminals) 
    {
      auto curSubRow = corerow->subrows.back();
      if(terminal->y > corerow->y || terminal->y + terminal->height <= corerow->y) { continue; }
      int tLeftX = terminal->x, tRightX = terminal->x + terminal->width;
      if(curSubRow->leftX >= tLeftX) 
      {
        if(curSubRow->rightX > tRightX) 
        {
          if(curSubRow->leftX <= tRightX) // curSubRow overlaps terminal
          {
            curSubRow->leftX = tRightX;
          }
        }
        else 
        {
          delete curSubRow;
          corerow->subrows.pop_back();
        }
      }
      else 
      {
        if(curSubRow->rightX > tRightX) 
        {
          corerow->subrows.emplace_back(new SubRow(tRightX, curSubRow->rightX));
          curSubRow->rightX = tLeftX;
        }
        else 
        {
          curSubRow->rightX = tLeftX;
        }
      }
    }
  }
}

void AbacusLegalizer::locateCellCorerow(Node* cell, int &corerowIdx)
{
  double dist {numeric_limits<double>::max()};
  for(int i = 0; i < input->block.size(); ++i)
  {
    double cur_dist = fabs(cell->y - input->block[i]->y);
    if(cur_dist < dist) 
    {
      dist = cur_dist;
      corerowIdx = i;
    }
    else 
    {
      break;
    }
  }
  corerowIdx = input->block.size()-1;
}

void AbacusLegalizer::locateCellSubrow(CoreRow* corerow, Node* cell, int &subrowIdx)
{
  if(corerow->subrows.empty())
  {
    subrowIdx = -1;
    return;
  } 

  // Try last subrow first
  if(cell->x >= corerow->subrows.back()->rightX && cell->width <= corerow->subrows.back()->capacity)
  {
    subrowIdx = corerow->subrows.size()-1;
    return;
  }

  for(int i = 0; i < corerow->subrows.size(); ++i) 
  {
    auto curSubrow = corerow->subrows[i];
    if(cell->x >= curSubrow->rightX) { continue; }
    if(cell->x >= curSubrow->leftX) 
    {
      if(cell->width <= curSubrow->capacity)
      {  
        subrowIdx = i;
        return;
      }
      else
      {
        if(i+1 < corerow->subrows.size() && cell->width <= corerow->subrows[i+1]->capacity)
        {
          subrowIdx = i+1;
          return;
        }
      }
    }
    else 
    {
      if(i == 0) { subrowIdx = 0; }
      else // cell is inside certain terminal
      {
        // Determine which way (Left:i-1; Right:i) to move first
        auto left_dist = cell->x - corerow->subrows[i-1]->rightX + cell->width;
        auto right_dist = curSubrow->leftX - cell->x;
        if(left_dist < right_dist)
        {
          // Try to insert left subrow first
          if(cell->width <= corerow->subrows[i-1]->capacity)
          {
            subrowIdx = i-1;
            return;
          }
          else if(cell->width <= curSubrow->capacity)
          {
            subrowIdx = i;
            return;
          }
        }
        else
        {
          // Try to insert right subrow first
          if(cell->width <= curSubrow->capacity)
          {
            subrowIdx = i;
            return;
          }
          else if(cell->width <= corerow->subrows[i-1]->capacity)
          {
            subrowIdx = i-1;
            return;
          }
        }
      }
    }
  }
  subrowIdx = -1;
  return;
}