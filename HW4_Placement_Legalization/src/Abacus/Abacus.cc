#include "Abacus.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <iostream>
using namespace std;

bool cmp(Cell const *A, Cell const *B)
{
    return A->x < B->x;
}

void Legalizer::divideRow()
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
          if(curSubrow->x_min <= t_max_x) // curSubrow overlaps terminal
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

int Legalizer::getRowIdx(Cell const *cell)
{
	double min_dist = numeric_limits<double>::max();
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

int Legalizer::getSubRowIdx(Row const *row, Cell const *cell)
{
	if(row->subRows.empty())	return -1;

	if(cell->x >= row->subRows.back()->x_max && cell->width <= row->subRows.back()->capacity)
	{
		return row->subRows.size() - 1;
	}
	else
	{
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
	}
	return -1;
}

void Legalizer::addCell(Cluster* cluster, Cell* cell, double optimalX)
{
  cluster->member.emplace_back(cell);
  cluster->weight += cell->weight;
  cluster->q += cell->weight * (optimalX - cluster->width);
  cluster->width += cell->width; 
}

// Add c into its predecessor cPrime
void Legalizer::addCluster(Cluster* prevCluster, Cluster* cluster)
{
  prevCluster->member.insert(prevCluster->member.end(), cluster->member.begin(), cluster->member.end());
  prevCluster->weight += cluster->weight;
  prevCluster->q += cluster->q - cluster->weight * prevCluster->width;
  prevCluster->width += cluster->width;
}

Cluster* Legalizer::collapse(Cluster* cluster, SubRow* subRow)
{
	// Place cluster c
	cluster->x = cluster->q / cluster->weight;
	// Limit position between x_min and x_max - w_c(c)
	if (cluster->x < subRow->x_min)
			cluster->x = subRow->x_min;
	if (cluster->x > subRow->x_max - cluster->width)
			cluster->x = subRow->x_max - cluster->width;
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

void Legalizer::placeRowTrail(int const &rowIdx, int const &subrowIdx, Cell *cell)
{
	auto const &row = input->rowList.at(rowIdx);
	if(subrowIdx == -1)
	{
		cell->optimalX = cell->optimalY = numeric_limits<double>::max();
		return;
	}

	auto const &subRow = row->subRows[subrowIdx];
	double optimalX = cell->x;
	if (cell->x < subRow->x_min)
			optimalX = subRow->x_min;
	else if (cell->x > subRow->x_max - cell->width)
			optimalX = subRow->x_max - cell->width;

	auto cluster = subRow->lastCluster;
	if (cluster == nullptr || cluster->x + cluster->width <= optimalX)
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

void Legalizer::placeRowFinal(int const &rowIdx, int const &subRowIdx, Cell *cell)
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


double Legalizer::calCost(Cell const *cell)
{
    if (cell->optimalX == numeric_limits<double>::max() ||
        cell->optimalY == numeric_limits<double>::max())
        return numeric_limits<double>::max();

    double x = cell->optimalX - cell->x,
           y = cell->optimalY - cell->y;
    return sqrt(x * x + y * y);
}

void Legalizer::determinePosition()
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
                    cluster->x = floor(shiftX / siteWidth) * siteWidth + subRow->x_min;
                else
                    cluster->x = ceil(shiftX / siteWidth) * siteWidth + subRow->x_min;

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

void Legalizer::abacusProcess()
{
    sort(input->cellList.begin(), input->cellList.end(), cmp);
    for (auto const &cell : input->cellList)
    {
			int bestRowIdx = getRowIdx(cell),
					bestSubRowIdx = getSubRowIdx(input->rowList[bestRowIdx], cell);
			placeRowTrail(bestRowIdx, bestSubRowIdx, cell);
			double bestCost = calCost(cell);
			int downFinder = bestRowIdx-1, upFinder = bestRowIdx+1;

			while (downFinder > 0 && abs(cell->y - input->rowList[downFinder]->y) < bestCost)
			{
				int lowSubrowIdx = getSubRowIdx(input->rowList[downFinder], cell);
				placeRowTrail(downFinder, lowSubrowIdx, cell);
				double cost = calCost(cell);
				if (cost < bestCost)
				{
						bestRowIdx = downFinder;
						bestSubRowIdx = lowSubrowIdx;
						bestCost = cost;
				}
				--downFinder ;
			}

			while (upFinder < input->rowList.size() - 1 && abs(cell->y - input->rowList[upFinder]->y) < bestCost)
			{
				int upSubrowIdx = getSubRowIdx(input->rowList[upFinder], cell);
				placeRowTrail(upFinder, upSubrowIdx, cell);
				double cost = calCost(cell);
				if (cost < bestCost)
				{
						bestRowIdx = upFinder;
						bestSubRowIdx = upSubrowIdx;
						bestCost = cost;
				}
				++upFinder;
			}
        placeRowFinal(bestRowIdx, bestSubRowIdx, cell);
    }
    determinePosition();
}

void Legalizer::calDisplacement()
{
    double sum = 0, max = 0;
    for (auto const &cell : input->cellList)
    {
        double x = cell->optimalX - cell->x,
               y = cell->optimalY - cell->y,
               dis = sqrt(x * x + y * y);
        sum += dis;
        if (max < dis)
            max = dis;
    }
    printf("total displacement: %.2f\n", sum);
    printf("max displacement:   %.2f\n", max);
}

ResultWriter *Legalizer::solve()
{
    divideRow();
    abacusProcess();
    calDisplacement();

    return new ResultWriter(input);
}
