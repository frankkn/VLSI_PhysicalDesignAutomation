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
      auto curSubrow = corerow->subrows.back();
      if(terminal->y > corerow->y || terminal->y + terminal->height <= corerow->y) { continue; }
      int tLeftX = terminal->x, tRightX = terminal->x + terminal->width;
      if(curSubrow->leftX >= tLeftX) 
      {
        if(curSubrow->rightX > tRightX) 
        {
          if(curSubrow->leftX <= tRightX) // curSubrow overlaps terminal
          {
            curSubrow->updateInfo(tRightX, curSubrow->rightX);
          }
        }
        else 
        {
          delete curSubrow;
          corerow->subrows.pop_back();
        }
      }
      else 
      {
        if(curSubrow->rightX > tRightX) 
        {
          corerow->subrows.emplace_back(new SubRow(tRightX, curSubrow->rightX));
          curSubrow->updateInfo(curSubrow->leftX, tLeftX);
        }
        else 
        {
          curSubrow->updateInfo(curSubrow->leftX, tLeftX);
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
      if(i == 0)
      { 
        if(cell->width <= curSubrow->capacity)
        {
          subrowIdx = 0;
        }
        else if(i+1 < corerow->subrows.size() && cell->width <= corerow->subrows[i+1]->capacity)
        {
          subrowIdx = 1;
        }
      }
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

void AbacusLegalizer::addCell(Cluster* c, Node* i)
{
  c->nodes.emplace_back(i);
  c->e_c += i->e;
  c->q_c += i->e * (i->bestX - c->w_c);
  c->w_c += i->width; 
}

// Add c into its predecessor cPrime
void AbacusLegalizer::addCluster(Cluster* cPrime, Cluster* c)
{
  cPrime->nodes.insert(cPrime->nodes.end(), c->nodes.begin(), c->nodes.end());
  cPrime->e_c += c->e_c;
  cPrime->q_c += c->q_c - c->e_c * cPrime->w_c; 
  cPrime->w_c += c->w_c;
}

void AbacusLegalizer::collapse(Cluster* c, SubRow* s)
{
  // Place cluster c
  c->x_c = c->q_c / c->e_c;

  // Limit position between subrow->min and subrow->max - c->width
  if(c->x_c < s->leftX)
  {
    c->x_c = s->leftX;
  }
  if(c->x_c > s->rightX - c->w_c)
  {
    c->x_c = s->rightX - c->w_c;
  }

  // Overlap between c and its predecessor c'(cPrime)?
  auto cPrime = c->prevC;
  if(cPrime != nullptr && cPrime->x_c + cPrime->w_c > c->x_c)
  {
    addCluster(cPrime, c);
    delete c;
    c = cPrime;
    collapse(cPrime, s);
  }
}

void AbacusLegalizer::placeTrialRow(Node* cell, int &corerowIdx, int &subrowIdx)
{
  if(subrowIdx == -1)
  {
    cell->bestX = cell->bestY = numeric_limits<double>::max();
    return;
  }

  // place cell into subrow 
  auto cursubrow = input->block[corerowIdx]->subrows[subrowIdx];
  double trialX = cell->x;
  if(cell->x < cursubrow->leftX) // stick within left bound of subrow
  {
    trialX = cursubrow->leftX;
  }
  else if(cell->x > cursubrow->rightX - cell->width) // stick to the right bound - cell->width of subrow 
  {
    trialX = cursubrow->rightX - cell->width;
  }

  auto curLastCluster = cursubrow->lastCluster;
  if(curLastCluster == nullptr || curLastCluster->x_c + curLastCluster->w_c <= cell->bestX)
  {
    // curLastCluster = new Cluster(cell->bestX, 0.0, 0.0, 0.0, curLastCluster);
    // addCell(curLastCluster, cell);
    cell->bestX = trialX;
    // cursubrow->lastCluster = curLastCluster;
  }
  else
  {
    addCell(curLastCluster, cell);
    collapse(curLastCluster, cursubrow);
    cell->bestX = curLastCluster->x_c + curLastCluster->w_c - cell->width;
    cursubrow->lastCluster = curLastCluster;
  }
  cell->bestY = input->block[corerowIdx]->y;
}

double AbacusLegalizer::determineCost(Node* cell)
{
  if(cell->bestX == numeric_limits<double>::max() || cell->bestY == numeric_limits<double>::max())
  {
    return numeric_limits<double>::max();
  }
  double displacementX = cell->bestX - cell->x;
  double displacementY = cell->bestY - cell->y;
  double euclideanDist = sqrt(displacementX * displacementX + displacementY * displacementY);
  return euclideanDist;
}

void AbacusLegalizer::placeFinalRow(Node* cell, int &bestCorerowIdx, int &bestSubrowIdx)
{
  // place cell into subrow 
  auto cursubrow = input->block[bestCorerowIdx]->subrows[bestSubrowIdx];
  double bestX = cell->x;
  if(cell->x < cursubrow->leftX) // stick within left bound of subrow
  {
    bestX = cursubrow->leftX;
  }
  else if(cell->x > cursubrow->rightX - cell->width) // stick to the right bound - cell->width of subrow 
  {
    bestX = cursubrow->rightX - cell->width;
  }
  cursubrow->capacity -= cell->width;

  auto cluster = cursubrow->lastCluster;
  if(cluster == nullptr || cluster->x_c + cluster->w_c <= bestX)
  {
    cell->bestX = bestX;
    // cluster = new Cluster(bestX, cell->e, cell->e * bestX, cell->width, cursubrow->lastCluster);
    // cursubrow->lastCluster = cluster; cluster->nodes.emplace_back(cell);
    cluster = new Cluster(bestX, 0, 0.0, 0, cursubrow->lastCluster);
    addCell(cluster, cell);
    cursubrow->lastCluster = cluster;
  }
  else
  {
    cell->bestX = bestX;
    addCell(cluster, cell);
    collapse(cluster, cursubrow);
    cursubrow->lastCluster = cluster;
  }
}

void AbacusLegalizer::transformPosition()
{
  for(auto &corerow: input->block)
  {
    int siteWidth = corerow->siteWidth;
    for(auto &subrow: corerow->subrows)
    {
      auto cluster = subrow->lastCluster;
      while(cluster != nullptr)
      {
        double displacementX = cluster->x_c - subrow->leftX;
        if(displacementX - floor(displacementX / siteWidth) * siteWidth <= siteWidth / 2.0)
        {
          cluster->x_c = floor(displacementX / siteWidth) * siteWidth + subrow->leftX;
        }
        else
        {
          cluster->x_c = ceil(displacementX / siteWidth) * siteWidth + subrow->leftX;
        }

        int bestX = cluster->x_c;
        for(auto &cell: cluster->nodes)
        {
          cell->bestX = bestX;
          cell->bestY = corerow->y;
          bestX += cell->width;
        }
        cluster = cluster->prevC;
      }
    }
  }
}

void AbacusLegalizer::abacusDP()
{
  sort(input->cells.begin(), input->cells.end(), [&](auto &a, auto &b){return a->x < b->x; });
  for(auto cell: input->cells)
  {
    double cBest {numeric_limits<double>::max()};
    int bestCorerowIdx = -1;
    locateCellCorerow(cell, bestCorerowIdx);
    int bestSubrowIdx = -1;
    locateCellSubrow(input->block[bestCorerowIdx], cell, bestSubrowIdx);
    
    // Current row cost:
    placeTrialRow(cell, bestCorerowIdx, bestSubrowIdx);
    double curCost = determineCost(cell);
    cBest = curCost < cBest? curCost: cBest;
    
    // Upper rows cost:
    int upRowIdx = bestCorerowIdx+1;int lowRowIdx = bestCorerowIdx-1;
    while(upRowIdx < input->block.size() && fabs(input->block[upRowIdx]->y - cell->y) < cBest)
    {
      int upSubrowIdx = -1;
      locateCellSubrow(input->block[upRowIdx], cell, upSubrowIdx);
      placeTrialRow(cell, upRowIdx, upSubrowIdx);
      double curCost = determineCost(cell);
      if(curCost < cBest)
      {
        bestCorerowIdx = upRowIdx;
        bestSubrowIdx = upSubrowIdx;
        cBest = curCost;
      }
      ++upRowIdx;
    }
    // Lower rows cost:
    while(lowRowIdx >= 0 && fabs(cell->y - input->block[lowRowIdx]->y) < cBest)
    {
      int lowSubrowIdx = -1;
      locateCellSubrow(input->block[lowRowIdx], cell, lowSubrowIdx);
      placeTrialRow(cell, lowRowIdx, lowSubrowIdx);
      double curCost = determineCost(cell);
      if(curCost < cBest)
      {
        bestCorerowIdx = lowRowIdx;
        bestSubrowIdx = lowSubrowIdx;
        cBest = curCost;
      }
      --lowRowIdx;
    }
    placeFinalRow(cell, bestCorerowIdx, bestSubrowIdx);
  }
  transformPosition();
}

OutputWriter* AbacusLegalizer::run()
{
  cutSubRow();
  abacusDP();
  return new OutputWriter(input);
}