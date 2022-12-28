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
	sort(input->terminalList.begin(), input->terminalList.end(), [&](auto&a, auto&b){ return a->x_global < b->x_global; });
	for(auto &row: input->rowList)
	{
		for(auto &terminal: input->terminalList)
    {
      auto curSubrow = row->subrows.back();
      if(terminal->y_global > row->y || terminal->y_global + terminal->height <= row->y) { continue; }
      int t_min_x = terminal->x_global, t_max_x = terminal->x_global + terminal->width;
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
		double cur_dist = abs(cell->y_global - input->rowList[i]->y);
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

int AbacusLegalizer::locateCellSubRow(Cell *cell, Row *row)
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
		if(cell->x_global >= curSubrow->x_max)	continue;
		if(cell->x_global >= curSubrow->x_min)
		{
			if(cell->width <= curSubrow->capacity)
			{
				return i;
			}
			// if(i+1 < row->subrows.size() && cell->width <= row->subrows[i+1]->capacity)
			// {
			// 	return i+1;
			// }
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
				auto left_dist = cell->x_global - row->subrows[i-1]->x_max + cell->width;
				auto right_dist = curSubrow->x_min - cell->x_global;
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

void AbacusLegalizer::addCell(Cluster *cluster, Cell *cell, double x_final)
{
  cluster->nodes.emplace_back(cell);
  cluster->e_c += cell->weight;
  cluster->q_c += cell->weight*(x_final - cluster->w_c);
  cluster->w_c += cell->width;
}

// Add c into its predecessor cPrime
void AbacusLegalizer::addCluster(Cluster *prevCluster, Cluster *cluster)
{
  prevCluster->nodes.insert(prevCluster->nodes.end(), cluster->nodes.begin(), cluster->nodes.end());
  prevCluster->e_c += cluster->e_c;
  prevCluster->q_c += cluster->q_c - cluster->e_c * prevCluster->w_c;
  prevCluster->w_c += cluster->w_c;
}

Cluster* AbacusLegalizer::collapse(Cluster *cluster, Subrow *subrow)
{
	// Place cluster c
	cluster->x_c = cluster->q_c / cluster->e_c;
	// Limit position between x_min and x_max - w_c(c)
	if (cluster->x_c < subrow->x_min)
	{
		cluster->x_c = subrow->x_min;
	}
	if (cluster->x_c > subrow->x_max - cluster->w_c)
	{
		cluster->x_c = subrow->x_max - cluster->w_c;
	}
	// Overlap between c and its predecessor c'?
	auto prevCluster = cluster->prevCluster;
	if (prevCluster != nullptr && prevCluster->x_c + prevCluster->w_c > cluster->x_c)
	{
    addCluster(prevCluster, cluster);
		delete cluster;
		cluster = collapse(prevCluster, subrow);
  }
	return cluster;
}

void AbacusLegalizer::addVirtualCell(Cell * cell, Cluster *cluster, double x_final, Subrow *subrow)
{	
	//Simulate adding cell into cluster
	int cur_weight = cluster->e_c + cell->weight;
	double cur_q = cluster->q_c + cell->weight * (x_final - cluster->w_c);
	int cur_width = cluster->w_c + cell->width;
	double cur_x = 0.0;
	while(1)
	{
		cur_x = cur_q / cur_weight;
		if(cur_x < subrow->x_min)
		{
			cur_x = subrow->x_min;
		}
		if(cur_x > subrow->x_max - cur_width)
		{		
			cur_x = subrow->x_max - cur_width;
		}
		auto const &prevCluster = cluster->prevCluster;
		if(prevCluster != nullptr && prevCluster->x_c + prevCluster->w_c > cur_x)
		{
			cur_weight = prevCluster->e_c + cur_weight;
			cur_q = prevCluster->q_c + cur_q - cur_weight * prevCluster->w_c;
			cur_width = prevCluster->w_c + cur_width;
			cluster = prevCluster;
		}
		else
		{
			break;
		}
	}
	cell->x_final = cur_x + cur_width - cell->width;
}

void AbacusLegalizer::placeTrialRow(Cell *cell, int &rowIdx, int &subrowIdx)
{
	auto const &row = input->rowList.at(rowIdx);
	if(subrowIdx == -1)
	{
		cell->x_final = cell->y_final = DBL_MAX;
		return;
	}

	auto const &subrow = row->subrows[subrowIdx];
	double x_final = cell->x_global;
	if(cell->x_global < subrow->x_min)
	{
		x_final = subrow->x_min;
	}
	else if(cell->x_global > subrow->x_max - cell->width)
	{
		x_final = subrow->x_max - cell->width;
	}
	auto cluster = subrow->lastCluster;
	if(cluster == nullptr || cluster->x_c + cluster->w_c <= x_final)
	{
		cell->x_final = x_final;
	}
	else
	{
		addVirtualCell(cell, cluster, x_final, subrow);
	}
	cell->y_final = row->y;
}

void AbacusLegalizer::placeFinalRow(Cell *cell, int &rowIdx, int &subRowIdx)
{
	auto subrow = input->rowList[rowIdx]->subrows[subRowIdx];
	subrow->capacity -= cell->width;

	double x_final = cell->x_global;
	if(cell->x_global < subrow->x_min)
		x_final = subrow->x_min;
	else if (cell->x_global > subrow->x_max - cell->width)
		x_final = subrow->x_max - cell->width;

	auto cluster = subrow->lastCluster;
	if(cluster == nullptr || cluster->x_c + cluster->w_c <= x_final)
	{
		cluster = new Cluster(subrow->lastCluster, x_final, 0, 0, 0);
		addCell(cluster, cell, x_final);
		subrow->lastCluster = cluster;
	}
	else
	{
		addCell(cluster, cell, x_final);
		subrow->lastCluster = collapse(cluster, subrow);
	}
}

double AbacusLegalizer::determineCost(Cell *cell)
{
	if (cell->x_final == DBL_MAX||cell->y_final == DBL_MAX)
	{
		return DBL_MAX;
	}
	double x_displacement = cell->x_final - cell->x_global, y_displacement = cell->y_final - cell->y_global;
	return sqrt(x_displacement * x_displacement + y_displacement * y_displacement);
}

void AbacusLegalizer::transformPosition()
{
	for (auto const &row: input->rowList)
	{
		int siteWidth = row->width;
		for (auto const &subrow: row->subrows)
		{
			auto cluster = subrow->lastCluster;
			while(cluster!= nullptr)
			{
				double x_displacement = cluster->x_c - subrow->x_min;
				int nearest_site = floor(x_displacement / siteWidth) * siteWidth;
				// cluster is closer to left site 
				if(x_displacement - nearest_site <= siteWidth/2)
				{
					cluster->x_c = floor(x_displacement / siteWidth) * siteWidth + subrow->x_min;
				}
				// cluster is closer to right site
				else
				{
					cluster->x_c = ceil(x_displacement / siteWidth) * siteWidth + subrow->x_min;
				}
				int x_final = cluster->x_c;
				for(auto &cell : cluster->nodes)
				{
					cell->x_final = x_final;
					cell->y_final = row->y;
					x_final += cell->width;
				}
				cluster = cluster->prevCluster;
			}
		}
	}
}

void AbacusLegalizer::searchUpRow(Cell* cell, int &upRowIdx, int &bestRowIdx, int &bestSubrowIdx, double &cBest)
{
	while(upRowIdx < input->rowList.size()-1 && abs(cell->y_global - input->rowList[upRowIdx]->y) < cBest)
	{
		int upSubrowIdx = locateCellSubRow(cell, input->rowList[upRowIdx]);
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
	while(downRowIdx>0 && abs(cell->y_global - input->rowList[downRowIdx]->y) < cBest)
	{
		int downSubrowIdx = locateCellSubRow(cell, input->rowList[downRowIdx]);
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
	sort(input->cellList.begin(), input->cellList.end(), [&](auto &a, auto&b){ return a->x_global < b->x_global;});
	for(auto const &cell : input->cellList)
	{
		int bestRowIdx = locateCellRow(cell);
		int bestSubrowIdx = locateCellSubRow(cell, input->rowList[bestRowIdx]);
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
		double x = cell->x_final - cell->x_global;
		double y = cell->y_final - cell->y_global;
		double euclideanDist = sqrt(x*x + y*y);
		totalDist += euclideanDist;
		if(maxDist < euclideanDist)
		{
			maxDist = euclideanDist;
		}
	}
	printf("Total displacement of cells = %.2f\n", totalDist);
	printf("Max displacement of cells   = %.2f\n", maxDist);
}

OutputWriter *AbacusLegalizer::run()
{
	cutSubRow();
	abacusDP();
	return new OutputWriter(input);
}
