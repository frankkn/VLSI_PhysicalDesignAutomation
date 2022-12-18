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

void AbacusLegalizer::placeTrialRow1(Node* cell, int &rowIdx, int &subRowIdx)
{
  if(subRowIdx == -1)
  {
    cell->bestX = cell->bestY = numeric_limits<double>::max();
    return;
  }

  auto const &subRow = input->block[rowIdx]->subrows[subRowIdx];
  double optimalX = cell->x;
  if (cell->x < subRow->leftX)
    optimalX = subRow->leftX;
  else if (cell->x > subRow->rightX - cell->width)
    optimalX = subRow->rightX - cell->width;

  auto cluster = subRow->lastCluster;
  if(cluster == nullptr || cluster->x_c + cluster->w_c <= optimalX)
  {
    cell->bestX = optimalX;
  }
  else
  {
    int trialWeight = cluster->w_c + cell->e;
    double trialQ = cluster->q_c + cell->e * (optimalX - cluster->w_c);
    int trialWidth = cluster->w_c + cell->width;

    double trialX = 0;
    while(1)
    {
      trialX = trialQ/ trialWeight;

      if(trialX < subRow->leftX)
        trialX = subRow->leftX;
      if(trialX > subRow->rightX - trialWidth)
        trialX = subRow->rightX - trialWidth;

      auto const &prevCluster = cluster->prevC;
      if(prevCluster != nullptr && prevCluster->x_c + prevCluster->w_c > trialX)
      {
        trialQ = prevCluster->q_c + trialQ - trialWeight * prevCluster->w_c;
        trialWeight = prevCluster->e_c + trialWeight;
        trialWidth = prevCluster->w_c + trialWidth;

        cluster = prevCluster;
      }
      else
      {
        break;
      }
    }
    cell->bestX = trialX + trialWidth - cell->width;
  }
  cell->bestY = input->block[rowIdx]->y;
}

void AbacusLegalizer::placeTrialRow2(Node* cell, int &corerowIdx, int &subrowIdx)
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

void AbacusLegalizer::placeFinalRow1(Node* cell, int &bestCorerowIdx, int &bestSubrowIdx)
{
  auto subRow = input->block[bestCorerowIdx]->subrows[bestSubrowIdx];
  subRow->capacity -= cell->width;

  double optimalX = cell->x;
  if(cell->x < subRow->leftX)
  {
    optimalX = subRow->leftX;
  }
  else if(cell->x > subRow->rightX - cell->width)
  {
    optimalX = subRow->rightX - cell->width;
  }

  auto cluster = subRow->lastCluster;
  if(cluster == nullptr || cluster->x_c + cluster->w_c <= optimalX)
  {
    cluster = new Cluster(optimalX, cell->e, cell->e * optimalX, cell->width, subRow->lastCluster);
    subRow->lastCluster = cluster;
    cluster->nodes.emplace_back(cell);
  }
  else
  {
    cluster->nodes.emplace_back(cell);
    cluster->e_c += cell->e;
    cluster->q_c += cell->e * (optimalX - cluster->w_c);
    cluster->w_c += cell->width;
    while(1)
    {
      cluster->x_c = cluster->q_c / cluster->e_c;

      if(cluster->x_c < subRow->leftX)
        cluster->x_c = subRow->leftX;
      if(cluster->x_c  > subRow->rightX - cluster->w_c)
        cluster->x_c = subRow->rightX - cluster->w_c;

      auto prevCluster = cluster->prevC;
      if(prevCluster != nullptr && prevCluster->x_c + prevCluster->w_c > cluster->x_c)
      {
        prevCluster->nodes.insert(prevCluster->nodes.end(), cluster->nodes.begin(), cluster->nodes.end());
        prevCluster->e_c += cluster->e_c;
        prevCluster->q_c += cluster->q_c - cluster->e_c * prevCluster->w_c;
        prevCluster->w_c += cluster->w_c;

        delete cluster;
        cluster = prevCluster;
      }
      else
      {
        break;
      }
    }
    subRow->lastCluster = cluster;
  }
}

void AbacusLegalizer::placeFinalRow2(Node* cell, int &bestCorerowIdx, int &bestSubrowIdx)
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
    placeTrialRow1(cell, bestCorerowIdx, bestSubrowIdx);
    double curCost = determineCost(cell);
    cBest = curCost < cBest? curCost: cBest;
    
    // Upper rows cost:
    int upRowIdx = bestCorerowIdx+1;int lowRowIdx = bestCorerowIdx-1;
    while(upRowIdx < input->block.size() && fabs(input->block[upRowIdx]->y - cell->y) < cBest)
    {
      int upSubrowIdx = -1;
      locateCellSubrow(input->block[upRowIdx], cell, upSubrowIdx);
      placeTrialRow1(cell, upRowIdx, upSubrowIdx);
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
      placeTrialRow1(cell, lowRowIdx, lowSubrowIdx);
      double curCost = determineCost(cell);
      if(curCost < cBest)
      {
        bestCorerowIdx = lowRowIdx;
        bestSubrowIdx = lowSubrowIdx;
        cBest = curCost;
      }
      --lowRowIdx;
    }
    placeFinalRow1(cell, bestCorerowIdx, bestSubrowIdx);
  }
  transformPosition();
}

OutputWriter* AbacusLegalizer::run()
{
  cutSubRow();
  abacusDP();
  return new OutputWriter(input);
}

/*
bool cmp(Node const *A, Node const *B)
{
    return A->x < B->x;
}

void AbacusLegalizer::divideRow()
{
  sort(input->terminals.begin(), input->terminals.end(), cmp);
  for (auto const &terminal : input->terminals)
  {
    int terminalMinX = terminal->x, terminalMaxX = terminal->x + terminal->width;
    for (auto const &row : input->block)
    {
      if (row->y < terminal->y || row->y >= terminal->y + terminal->height)
        continue;

      auto lastSubRow = row->subrows.back();
      if (lastSubRow->leftX < terminalMinX)
      {
        if (lastSubRow->rightX > terminalMaxX)
          row->subrows.emplace_back(new SubRow(terminalMaxX, lastSubRow->rightX));
        lastSubRow->updateInfo(lastSubRow->leftX, terminalMinX);
      }
      else
      {
        if (lastSubRow->rightX > terminalMaxX)
        {
          lastSubRow->updateInfo(terminalMaxX, lastSubRow->rightX);
        }
        else
        {
          delete lastSubRow;
          row->subrows.pop_back();
        }
      }
    }
  }
}

int AbacusLegalizer::getRowIdx(Node* cell)
{
  double minY = numeric_limits<double>::max();
  for(int i = 0; i < input->block.size(); ++i)
  {
    if(minY > abs(cell->y - input->block[i]->y))
    {
      minY = abs(cell->y - input->block[i]->y);
    }
    else
    {
      return i;
    }
  }
  return input->block.size()-1;
}

int AbacusLegalizer::getSubRowIdx(CoreRow* row, Node* cell)
{
  auto const &subRows = row->subrows;
  if(subRows.empty() == true) return -1;
  if(cell->x >= row->subrows.back()->rightX)
  {
    if(cell->width <= row->subrows.back()->capacity)
    {
      return subRows.size()-1;
    }
  }
  else
  {
    for(int i = 0; i < subRows.size(); ++i)
    {
      auto subRow = row->subrows[i];
      if(cell->x >= subRow->rightX)
      {
        if(cell->width <= subRow->capacity)
        {
          return i;
        }
        if(i+1 < row->subrows.size() && cell->width <= row->subrows[i+1]->capacity)
        {
          return i+1;
        }
      }
      else
      {
        if(i > 0)
        {
          if(abs(cell->x + cell->width - row->subrows[i-1]->rightX) < abs(cell->x - subRow->leftX))
          {
            if(cell->width <= row->subrows[i-1]->capacity)
            {
              return i-1;
            }
            if(cell->width <= subRow->capacity)
            {
              return i;
            }
          }
          else
          {
            if(cell->width <= subRow->capacity)
            {
              return 0;
            }
          }
        }
      }
    }
  }
  return -1;
}

double AbacusLegalizer::calCost(Node *cell)
{
  if (cell->bestX == numeric_limits<double>::max() ||
      cell->bestY == numeric_limits<double>::max())
      return std::numeric_limits<double>::max();

  double x = cell->bestX - cell->x,
          y = cell->bestY - cell->y;
  return sqrt(x * x + y * y);
}

int AbacusLegalizer::placeRowTrail(int &rowIdx, Node *cell)
{
  auto const &row = input->block.at(rowIdx);
  int subRowIdx = getSubRowIdx(row, cell);
  if (subRowIdx == -1)
  {
    cell->bestX = cell->bestY = numeric_limits<double>::max();
    return -1;
  }

  auto const &subRow = row->subrows[subRowIdx];
  double optimalX = cell->x;
  if (cell->x < subRow->leftX)
    optimalX = subRow->leftX;
  else if (cell->x > subRow->rightX - cell->width)
    optimalX = subRow->rightX - cell->width;

  auto cluster = subRow->lastCluster;
  if (cluster == nullptr || cluster->x_c + cluster->w_c <= optimalX)
  {
    cell->bestX = optimalX;
  }
  else
  {
    int trialWeight = cluster->e_c + cell->e;
    double trialQ = cluster->q_c + cell->e * (optimalX - cluster->w_c);
    int trialWidth = cluster->w_c + cell->width;

    double trialX = 0;
    while (1)
    {
      trialX = trialQ / trialWeight;

      if (trialX < subRow->leftX)
        trialX = subRow->leftX;
      if (trialX > subRow->rightX - trialWidth)
        trialX = subRow->rightX - trialWidth;

      auto const &prevCluster = cluster->prevC;
      if (prevCluster != nullptr && prevCluster->x_c + prevCluster->w_c > trialX)
      {
        trialQ = prevCluster->q_c + trialQ - trialWeight * prevCluster->w_c;
        trialWeight = prevCluster->e_c + trialWeight;
        trialWidth = prevCluster->w_c + trialWidth;

        cluster = prevCluster;
      }
      else
      {
        break;
      }
    }
    cell->bestX = trialX + trialWidth - cell->width;
  }
  cell->bestY = row->y;
  return subRowIdx;
}

void AbacusLegalizer::placeRowFinal(int &rowIdx, int &subRowIdx, Node *cell)
{
  auto subRow = input->block[rowIdx]->subrows[subRowIdx];
  subRow->capacity -= cell->width;

  double optimalX = cell->x;
  if (cell->x < subRow->leftX)
    optimalX = subRow->leftX;
  else if (cell->x > subRow->rightX - cell->width)
    optimalX = subRow->rightX - cell->width;

  auto cluster = subRow->lastCluster;
  if (cluster == nullptr || cluster->x_c + cluster->w_c <= optimalX)
  {
    cluster = new Cluster(optimalX,
                          cell->e,
                          cell->e * optimalX,
                          cell->width,
                          subRow->lastCluster);
    subRow->lastCluster = cluster;
    cluster->nodes.emplace_back(cell);
  }
  else
  {
    cluster->nodes.emplace_back(cell);
    cluster->e_c += cell->e;
    cluster->q_c += cell->e * (optimalX - cluster->w_c);
    cluster->w_c += cell->width;

    while (true)
    {
      cluster->x_c = cluster->q_c / cluster->e_c;

      if (cluster->x_c < subRow->leftX)
        cluster->x_c = subRow->leftX;
      if (cluster->x_c > subRow->rightX - cluster->w_c)
        cluster->x_c = subRow->rightX - cluster->w_c;

      auto prevCluster = cluster->prevC;
      if (prevCluster != nullptr && prevCluster->x_c + prevCluster->w_c > cluster->x_c)
      {
        prevCluster->nodes.insert(prevCluster->nodes.end(), cluster->nodes.begin(), cluster->nodes.end());
        prevCluster->e_c += cluster->e_c;
        prevCluster->q_c += cluster->q_c - cluster->e_c * prevCluster->w_c;
        prevCluster->w_c += cluster->w_c;

        delete cluster;
        cluster = prevCluster;
      }
      else
      {
        break;
      }
    }
    subRow->lastCluster = cluster;
  }
}

void AbacusLegalizer::abacusProcess()
{
  sort(input->cells.begin(), input->cells.end(), cmp);
  for (auto const &cell : input->cells)
  {
    int bestRowIdx = getRowIdx(cell),
      bestSubRowIdx = placeRowTrail(bestRowIdx, cell);
    double bestCost = calCost(cell);
    int downFinder = bestRowIdx, upFinder = bestRowIdx;
    while (downFinder > 0 && std::abs(cell->y - input->block[downFinder]->y) < bestCost)
    {
      downFinder -= 1;
      int subRowIdx = placeRowTrail(downFinder, cell);
      double cost = calCost(cell);
      if (cost < bestCost)
      {
        bestRowIdx = downFinder;
        bestSubRowIdx = subRowIdx;
        bestCost = cost;
      }
    }
    while (upFinder < input->block.size() - 1 && std::abs(cell->y - input->block[upFinder]->y) < bestCost)
    {
      upFinder += 1;
      int subRowIdx = placeRowTrail(upFinder, cell);
      double cost = calCost(cell);
      if (cost < bestCost)
      {
        bestRowIdx = upFinder;
        bestSubRowIdx = subRowIdx;
        bestCost = cost;
      }
    }
    placeRowFinal(bestRowIdx, bestSubRowIdx, cell);
  }
  transformPosition();
}

OutputWriter* AbacusLegalizer::solve()
{
  divideRow();
  abacusProcess();
  // calDisplacement();
  return new OutputWriter(input);
}
*/