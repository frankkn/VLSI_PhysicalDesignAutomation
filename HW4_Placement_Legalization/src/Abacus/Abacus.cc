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
      auto curSubrow = row->subRows.back();
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
          row->subRows.pop_back();
        }
      }
      else
      {
        if(curSubrow->x_max > t_max_x)
        {
          row->subRows.emplace_back(new SubRow(t_max_x, curSubrow->x_max));
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
	if(row->subRows.empty())	return -1;
	// if(cell->x >= row->subRows.back()->x_max && cell->width <= row->subRows.back()->capacity)
	// {
	// 	return row->subRows.size()-1;
	// }
	// else
	// {
	for(int i = 0; i < row->subRows.size(); ++i)
	{
		auto curSubrow = row->subRows[i];
		if(cell->x >= curSubrow->x_max)	continue;
		if(cell->x >= curSubrow->x_min)
		{
			if(cell->width <= curSubrow->capacity)
			{
				return i;
			}
			if(i+1 < row->subRows.size() && cell->width <= row->subRows[i+1]->capacity)
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
				else if(i+1 < row->subRows.size() && cell->width <= row->subRows[i+1]->capacity)
				{
					return 1;
				}
			}
			else
			{
				// Determine which direction (Left:i-1; Right:i) to move
				auto left_dist = cell->x - row->subRows[i-1]->x_max + cell->width;
				auto right_dist = curSubrow->x_min - cell->x;
				if(left_dist < right_dist)
				{
					if(cell->width <= row->subRows[i-1]->capacity)
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
					else if(cell->width <= row->subRows[i-1]->capacity)
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

void AbacusLegalizer::addCell(Cluster* cluster, Cell* cell, double optimalX)
{
  cluster->member.emplace_back(cell);
  cluster->weight += cell->weight;
  cluster->q += cell->weight*(optimalX - cluster->width);
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

Cluster* AbacusLegalizer::collapse(Cluster* cluster, SubRow* subRow)
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
	auto prevCluster = cluster->predecessor;
	if (prevCluster != nullptr && prevCluster->x + prevCluster->width > cluster->x)
	{
    addCluster(prevCluster, cluster);
		delete cluster;
		cluster = collapse(prevCluster, subRow);
  }
	return cluster;
}

void AbacusLegalizer::placeTrialRow(int &rowIdx, int &subrowIdx, Cell *cell)
{
	auto const &row = input->rowList.at(rowIdx);
	if(subrowIdx == -1)
	{
		cell->optimalX = cell->optimalY = DBL_MAX;
		return;
	}

	auto const &subRow = row->subRows[subrowIdx];
	double optimalX = cell->x;
	if(cell->x < subRow->x_min)
	{
		optimalX = subRow->x_min;
	}
	else if(cell->x > subRow->x_max - cell->width)
	{
		optimalX = subRow->x_max - cell->width;
	}
	auto cluster = subRow->lastCluster;
	if(cluster == nullptr || cluster->x + cluster->width <= optimalX)
	{
		cell->optimalX = optimalX;
	}
	else
	{
		// Simulate adding cell into cluster
		int curWeight = cluster->weight + cell->weight;
		double curQ = cluster->q + cell->weight * (optimalX - cluster->width);
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
			auto const &prevCluster = cluster->predecessor;
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
		cell->optimalX = curX + curWidth - cell->width;
	}
	cell->optimalY = row->y;
}

void AbacusLegalizer::placeFinalRow(int &rowIdx, int &subRowIdx, Cell *cell)
{
	auto subRow = input->rowList[rowIdx]->subRows[subRowIdx];
	subRow->capacity -= cell->width;

	double optimalX = cell->x;
	if(cell->x < subRow->x_min)
		optimalX = subRow->x_min;
	else if (cell->x > subRow->x_max - cell->width)
		optimalX = subRow->x_max - cell->width;

	auto cluster = subRow->lastCluster;
	if(cluster == nullptr || cluster->x + cluster->width <= optimalX)
	{
		cluster = new Cluster(optimalX, subRow->lastCluster, 0, 0.0, 0);
		addCell(cluster, cell, optimalX);
		subRow->lastCluster = cluster;
	}
	else
	{
		addCell(cluster, cell, optimalX);
		subRow->lastCluster = collapse(cluster, subRow);
	}
}

double AbacusLegalizer::calCost(Cell *cell)
{
	if (cell->optimalX == DBL_MAX||cell->optimalY == DBL_MAX)
	{
		return DBL_MAX;
	}
	double x = cell->optimalX - cell->x, y = cell->optimalY - cell->y;
	return sqrt(x*x + y*y);
}

void AbacusLegalizer::transformPosition()
{
	for (auto const &row : input->rowList)
	{
		int siteWidth = row->width;
		for (auto const &subRow : row->subRows)
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
				int optimalX = cluster->x;
				for (auto &cell : cluster->member)
				{
					cell->optimalX = optimalX;
					cell->optimalY = row->y;
					optimalX += cell->width;
				}
				cluster = cluster->predecessor;
			}
		}
	}
}

void AbacusLegalizer::abacusDP()
{
	sort(input->cellList.begin(), input->cellList.end(), [&](auto &a, auto&b){ return a->x < b->x;});
	for (auto const &cell : input->cellList)
	{
		int bestRowIdx = locateCellRow(cell);
		int bestSubRowIdx = locateCellSubRow(input->rowList[bestRowIdx], cell);
		placeTrialRow(bestRowIdx, bestSubRowIdx, cell);
		double cBest = calCost(cell);

		int uplowIdx = bestRowIdx+1, lowRowIdx = bestRowIdx-1;
		while(uplowIdx < input->rowList.size()-1 && abs(cell->y - input->rowList[uplowIdx]->y) < cBest)
		{
			int upSubrowIdx = locateCellSubRow(input->rowList[uplowIdx], cell);
			placeTrialRow(uplowIdx, upSubrowIdx, cell);
			double curCost = calCost(cell);
			if (curCost < cBest)
			{
					bestRowIdx = uplowIdx;
					bestSubRowIdx = upSubrowIdx;
					cBest = curCost;
			}
			++uplowIdx;
		}
		while(lowRowIdx>0 && abs(cell->y - input->rowList[lowRowIdx]->y) < cBest)
		{
			int lowSubrowIdx = locateCellSubRow(input->rowList[lowRowIdx], cell);
			placeTrialRow(lowRowIdx, lowSubrowIdx, cell);
			double curCost = calCost(cell);
			if (curCost < cBest)
			{
					bestRowIdx = lowRowIdx;
					bestSubRowIdx = lowSubrowIdx;
					cBest = curCost;
			}
			--lowRowIdx ;
		}
		placeFinalRow(bestRowIdx, bestSubRowIdx, cell);
	}
	transformPosition();
}

void AbacusLegalizer::calDisplacement()
{
	double totalDist = 0, maxDist = 0;
	for(auto &cell: input->cellList)
	{
		double x = cell->optimalX - cell->x;
		double y = cell->optimalY - cell->y;
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
