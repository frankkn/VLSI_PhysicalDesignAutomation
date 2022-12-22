#pragma once
#include <string>
#include <vector>
using namespace std;

struct Cell
{
	string name;
	int width, height, weight;
	double globalX, globalY, finalX, finalY;

	Cell(string const &name, int const &width, int const &height)
		:name(name), width(width), height(height), weight(width), globalX(0), globalY(0), finalX(0), finalY(0) {}
};

struct Cluster
{
	double x, q;
	int width, weight;
	vector<Cell*> member;
	Cluster *prevCluster;

	Cluster(Cluster *prevCluster, double const &x, int const &weight, double const &q, int const &width)
		:prevCluster(prevCluster), x(x), q(q), width(width), weight(weight) {}
};

struct Subrow
{
	int x_min, x_max, capacity;
	Cluster *lastCluster;

	void updateInfo(int const &new_x_min, int const &new_x_max);

	Subrow(int const &x_min, int const &x_max):x_min(x_min), x_max(x_max), capacity(x_max - x_min), lastCluster(nullptr) {}
};

struct Row
{
	int sitwWidth, height, y;
	vector<Subrow*> subrows;

	Row(int &sitwWidth, int &height, int &y):sitwWidth(sitwWidth), height(height), y(y) {}
};

struct Input
{
	int maxDisplacement;
	vector<Cell *>cellList, terminalList;
	vector<Row *>rowList;

	Input(int &maxDisplacement, vector<Cell*> &cellList, vector<Cell*> &terminalList, vector<Row*> &rowList)
		:maxDisplacement(maxDisplacement), cellList(cellList), terminalList(terminalList), rowList(rowList) {}
};
