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
	Cluster* collapse(Cluster* c, SubRow* s);
	void placeTrialRow(int &rowIdx, int &subrowIdx,Cell *cell);
	void placeFinalRow(int &rowIdx, int &subRowIdx, Cell *cell);
	double calCost(Cell *cell);
	void transformPosition();
	void abacusDP();
	void calDisplacement();
public:
	AbacusLegalizer(Input *input):input(input) {}
	OutputWriter *run();
};
