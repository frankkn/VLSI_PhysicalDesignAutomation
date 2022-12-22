#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <limits>
#include "Abacus.h"
#define DBL_MAX numeric_limits<double>::max()
using namespace std;

void AbacusLegalizer::cutSubRow()
{
	sort(input->terminalList.begin(), input->terminalList.end(), [&](auto&a, auto&b){ return a->x < b->x; });
	for(auto &row: input->rowList)
	{
		for(auto &terminal: input->terminalList)
    {
      auto curSubrow = row->subrows.back();
      if(terminal->y > row->y || terminal->y + terminal->height <= row->y) { continue; }
      int t_min_x = terminal->x, t_max_x = terminal->x + terminal->width;
      if(curSubrow->x_min >= t_min_x)
      {
        if(curSubrow->x_max > t_max_x)
        {
          if(curSubrow->x_min <= t_max_x)// curSubrow overlaps terminal
          {
            curSubrow->updateInfo(t_max_x, curSubrow->x_max);
          }
        }
        else
        {
          delete curSubrow;
          row->subrows.pop_back();
        }
      }
      else
      {
        if(curSubrow->x_max > t_max_x)
        {
          row->subrows.emplace_back(new Subrow(t_max_x, curSubrow->x_max));
          curSubrow->updateInfo(curSubrow->x_min, t_min_x);
        }
        else
        {
          curSubrow->updateInfo(curSubrow->x_min, t_min_x);
        }
      }
    }
  }
}

int AbacusLegalizer::locateCellRow(Cell *cell)
{
	double min_dist = DBL_MAX;
	int row_idx = -1;
	for(int i = 0; i < input->rowList.size(); ++i)
	{
		double cur_dist = abs(cell->y - input->rowList[i]->y);
		if (cur_dist < min_dist)
		{
				min_dist = cur_dist;
				row_idx = i;
		}
		else
		{
			break;
		}
	}
	return row_idx;
}

int AbacusLegalizer::locateCellSubRow(Row *row, Cell *cell)
{
	if(row->subrows.empty())	return -1;
	// if(cell->x >= row->subrows.back()->x_max && cell->width <= row->subrows.back()->capacity)
	// {
	// 	return row->subrows.size()-1;
	// }
	// else
	// {
	for(int i = 0; i < row->subrows.size(); ++i)
	{
		auto curSubrow = row->subrows[i];
		if(cell->x >= curSubrow->x_max)	continue;
		if(cell->x >= curSubrow->x_min)
		{
			if(cell->width <= curSubrow->capacity)
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
			if(i == 0)
			{
				if(cell->width <= curSubrow->capacity)
				{
					return 0;
				}
				else if(i+1 < row->subrows.size() && cell->width <= row->subrows[i+1]->capacity)
				{
					return 1;
				}
			}
			else
			{
				// Determine which direction (Left:i-1; Right:i) to move
				auto left_dist = cell->x - row->subrows[i-1]->x_max + cell->width;
				auto right_dist = curSubrow->x_min - cell->x;
				if(left_dist < right_dist)
				{
					if(cell->width <= row->subrows[i-1]->capacity)
					{
						return i-1;
					}
					else if(cell->width <= curSubrow->capacity)
					{
						return i;
					}
				}
				else
				{
					if(cell->width <= curSubrow->capacity)
					{
						return i;
					}
					else if(cell->width <= row->subrows[i-1]->capacity)
					{
						return i-1;
					}
				}
			}
		}
	}
	// }
	return -1;
}

void AbacusLegalizer::addCell(Cluster* cluster, Cell* cell, double finalX)
{
  cluster->member.emplace_back(cell);
  cluster->weight += cell->weight;
  cluster->q += cell->weight*(finalX - cluster->width);
  cluster->width += cell->width;
}

// Add c into its predecessor cPrime
void AbacusLegalizer::addCluster(Cluster* prevCluster, Cluster* cluster)
{
  prevCluster->member.insert(prevCluster->member.end(), cluster->member.begin(), cluster->member.end());
  prevCluster->weight += cluster->weight;
  prevCluster->q += cluster->q - cluster->weight * prevCluster->width;
  prevCluster->width += cluster->width;
}

Cluster* AbacusLegalizer::collapse(Cluster* cluster, Subrow* subRow)
{
	// Place cluster c
	cluster->x = cluster->q / cluster->weight;
	// Limit position between x_min and x_max - w_c(c)
	if (cluster->x < subRow->x_min)
	{
		cluster->x = subRow->x_min;
	}
	if (cluster->x > subRow->x_max - cluster->width)
	{
		cluster->x = subRow->x_max - cluster->width;
	}
	// Overlap between c and its predecessor c'?
	auto prevCluster = cluster->prevCluster;
	if (prevCluster != nullptr && prevCluster->x + prevCluster->width > cluster->x)
	{
    addCluster(prevCluster, cluster);
		delete cluster;
		cluster = collapse(prevCluster, subRow);
  }
	return cluster;
}

void AbacusLegalizer::placeTrialRow(Cell *cell, int &rowIdx, int &subrowIdx)
{
	auto const &row = input->rowList.at(rowIdx);
	if(subrowIdx == -1)
	{
		cell->finalX = cell->finalY = DBL_MAX;
		return;
	}

	auto const &subRow = row->subrows[subrowIdx];
	double finalX = cell->x;
	if(cell->x < subRow->x_min)
	{
		finalX = subRow->x_min;
	}
	else if(cell->x > subRow->x_max - cell->width)
	{
		finalX = subRow->x_max - cell->width;
	}
	auto cluster = subRow->lastCluster;
	if(cluster == nullptr || cluster->x + cluster->width <= finalX)
	{
		cell->finalX = finalX;
	}
	else
	{
		// Simulate adding cell into cluster
		int curWeight = cluster->weight + cell->weight;
		double curQ = cluster->q + cell->weight * (finalX - cluster->width);
		int curWidth = cluster->width + cell->width;
		double curX = 0.0;
		while(1)
		{
			curX = curQ / curWeight;
			if(curX < subRow->x_min)
			{
				curX = subRow->x_min;
			}
			if(curX > subRow->x_max - curWidth)
			{		
				curX = subRow->x_max - curWidth;
			}
			auto const &prevCluster = cluster->prevCluster;
			if(prevCluster != nullptr && prevCluster->x + prevCluster->width > curX)
			{
				curQ = prevCluster->q + curQ - curWeight * prevCluster->width;
				curWeight = prevCluster->weight + curWeight;
				curWidth = prevCluster->width + curWidth;
				cluster = prevCluster;
			}
			else
			{
				break;
			}
		}
		cell->finalX = curX + curWidth - cell->width;
	}
	cell->finalY = row->y;
}

void AbacusLegalizer::placeFinalRow(Cell *cell, int &rowIdx, int &subRowIdx)
{
	auto subRow = input->rowList[rowIdx]->subrows[subRowIdx];
	subRow->capacity -= cell->width;

	double finalX = cell->x;
	if(cell->x < subRow->x_min)
		finalX = subRow->x_min;
	else if (cell->x > subRow->x_max - cell->width)
		finalX = subRow->x_max - cell->width;

	auto cluster = subRow->lastCluster;
	if(cluster == nullptr || cluster->x + cluster->width <= finalX)
	{
		cluster = new Cluster(subRow->lastCluster, finalX, 0, 0, 0);
		addCell(cluster, cell, finalX);
		subRow->lastCluster = cluster;
	}
	else
	{
		addCell(cluster, cell, finalX);
		subRow->lastCluster = collapse(cluster, subRow);
	}
}

double AbacusLegalizer::determineCost(Cell *cell)
{
	if (cell->finalX == DBL_MAX||cell->finalY == DBL_MAX)
	{
		return DBL_MAX;
	}
	double x = cell->finalX - cell->x, y = cell->finalY - cell->y;
	return sqrt(x*x + y*y);
}

void AbacusLegalizer::transformPosition()
{
	for (auto const &row : input->rowList)
	{
		int siteWidth = row->width;
		for (auto const &subRow : row->subrows)
		{
			auto cluster = subRow->lastCluster;
			while (cluster != nullptr)
			{
				double shiftX = cluster->x - subRow->x_min;
				if (shiftX - floor(shiftX / siteWidth) * siteWidth <= siteWidth / 2.0)
				{
					cluster->x = floor(shiftX / siteWidth) * siteWidth + subRow->x_min;
				}
				else
				{
					cluster->x = ceil(shiftX / siteWidth) * siteWidth + subRow->x_min;
				}
				int finalX = cluster->x;
				for (auto &cell : cluster->member)
				{
					cell->finalX = finalX;
					cell->finalY = row->y;
					finalX += cell->width;
				}
				cluster = cluster->prevCluster;
			}
		}
	}
}

void AbacusLegalizer::searchUpRow(Cell* cell, int &upRowIdx, int &bestRowIdx, int &bestSubrowIdx, double &cBest)
{
	while(upRowIdx < input->rowList.size()-1 && abs(cell->y - input->rowList[upRowIdx]->y) < cBest)
	{
		int upSubrowIdx = locateCellSubRow(input->rowList[upRowIdx], cell);
		placeTrialRow(cell, upRowIdx, upSubrowIdx);
		double curCost = determineCost(cell);
		if (curCost < cBest)
		{
				bestRowIdx = upRowIdx;
				bestSubrowIdx = upSubrowIdx;
				cBest = curCost;
		}
		++upRowIdx;
	}
}

void AbacusLegalizer::searchDownRow(Cell* cell, int &downRowIdx, int &bestRowIdx, int &bestSubrowIdx, double &cBest)
{
	while(downRowIdx>0 && abs(cell->y - input->rowList[downRowIdx]->y) < cBest)
	{
		int downSubrowIdx = locateCellSubRow(input->rowList[downRowIdx], cell);
		placeTrialRow(cell, downRowIdx, downSubrowIdx);
		double curCost = determineCost(cell);
		if(curCost < cBest)
		{
			bestRowIdx = downRowIdx;
			bestSubrowIdx = downSubrowIdx;
			cBest = curCost;
		}
		--downRowIdx;
	}
}

void AbacusLegalizer::abacusDP()
{
	sort(input->cellList.begin(), input->cellList.end(), [&](auto &a, auto&b){ return a->x < b->x;});
	for(auto const &cell : input->cellList)
	{
		int bestRowIdx = locateCellRow(cell);
		int bestSubrowIdx = locateCellSubRow(input->rowList[bestRowIdx], cell);
		placeTrialRow(cell, bestRowIdx, bestSubrowIdx);
		double cBest = determineCost(cell);
		int upRowIdx = bestRowIdx+1, downRowIdx = bestRowIdx-1;
		searchUpRow(cell, upRowIdx, bestRowIdx, bestSubrowIdx, cBest);
		searchDownRow(cell, downRowIdx, bestRowIdx, bestSubrowIdx, cBest);
		placeFinalRow(cell, bestRowIdx, bestSubrowIdx);
	}
	transformPosition();
}

void AbacusLegalizer::calDisplacement()
{
	double totalDist = 0, maxDist = 0;
	for(auto &cell: input->cellList)
	{
		double x = cell->finalX - cell->x;
		double y = cell->finalY - cell->y;
		double dis = sqrt(x*x + y*y);
		totalDist += dis;
		if(maxDist < dis)
		{
			maxDist = dis;
		}
	}
	printf("Max displacement of cells   = %.2f\n", maxDist);
	printf("Total displacement of cells = %.2f\n", totalDist);
}

OutputWriter *AbacusLegalizer::run()
{
	cutSubRow();
	abacusDP();
	calDisplacement();
	return new OutputWriter(input);
}
