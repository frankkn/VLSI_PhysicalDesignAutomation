#pragma once
#include <string>
#include <vector>
using namespace std;

struct Cell
{
	string name;
	int width, height, weight;
	double x, y, optimalX, optimalY;

	Cell(string const &name, int const &width, int const &height)
			: name(name), width(width), height(height), weight(width), x(0), y(0), optimalX(0), optimalY(0) {}
};

struct Cluster
{
	double x, q;
	int width, weight;
	vector<Cell*> member;
	Cluster *predecessor;

	Cluster(double const &x, Cluster *predecessor, int const &weight, double const &q, int const &width)
			: x(x), q(q), width(width), weight(weight), predecessor(predecessor) {}
};

struct SubRow
{
	int x_min, x_max, capacity;
	Cluster *lastCluster;

	void updateInfo(int const &new_x_min, int const &new_x_max);

	SubRow(int const &x_min, int const &x_max):x_min(x_min), x_max(x_max), capacity(x_max - x_min), lastCluster(nullptr) {}

};

struct Row
{
	int width, height, y;
	vector<SubRow*> subRows;

	Row(int &width, int &height, int &y):width(width), height(height), y(y) {}
};

struct Input
{
	int maxDisplacement;
	vector<Cell *>cellList, terminalList;
	vector<Row *>rowList;

	Input(int &maxDisplacement, vector<Cell*> &cellList, vector<Cell*> &terminalList, vector<Row*> &rowList)
		:maxDisplacement(maxDisplacement), cellList(cellList), terminalList(terminalList), rowList(rowList) {}
};
