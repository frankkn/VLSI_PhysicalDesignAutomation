#pragma once
#include "../OutputWriter/OutputWriter.h"
#include "../Module/Module.h"

class AbacusLegalizer
{
	Input *input;

	void cutSubRow();
	int locateCellRow(Cell *cell);
	int locateCellSubRow(Row *row, Cell *cell);
	void addCell(Cluster* c, Cell* i, double optimalX);
	void addCluster(Cluster* cPrime, Cluster* c);
	Cluster* collapse(Cluster* c, SubRow* s);
	void placeTrialRow(int const &rowIdx, int const &subrowIdx,Cell *cell);
	void placeFinalRow(int const &rowIdx, int const &subRowIdx, Cell *cell);
	double calCost(Cell const *cell);
	void transformPosition();
	void abacusDP();
	void calDisplacement();
public:
	AbacusLegalizer(Input *input):input(input) {}
	OutputWriter *run();
};
