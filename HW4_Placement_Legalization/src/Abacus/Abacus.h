#pragma once
#include "../Module/Module.h"
#include "../OutputWriter/OutputWriter.h"

class AbacusLegalizer
{
	Input *input;

	void cutSubRow();
	int locateCellRow(Cell *cell);
	int locateCellSubRow(Row *row, Cell *cell);
	void addCell(Cluster* c, Cell* i, double optimalX);
	void addCluster(Cluster* cPrime, Cluster* c);
	Cluster* collapse(Cluster* c, Subrow* s);
	void searchUpRow(Cell *cell, int &upRowIdx, int &bestRowIdx, int &bestSubrowIdx, double &cBest);
	void searchDownRow(Cell *cell, int &downRowIdx, int &bestRowIdx, int &bestSubrowIdx, double &cBest);
	void placeTrialRow(Cell *cell, int &rowIdx, int &subrowIdx);
	void placeFinalRow(Cell *cell, int &rowIdx, int &subRowIdx);
	double determineCost(Cell *cell);
	void transformPosition();
	void abacusDP();
	void calDisplacement();
public:
	AbacusLegalizer(Input *input):input(input) {}
	OutputWriter *run();
};
