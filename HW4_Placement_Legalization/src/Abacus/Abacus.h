#pragma once
#include "../Module/Module.h"
#include "../OutputWriter/OutputWriter.h"

class AbacusLegalizer
{
	Input *input;

	void cutSubRow();
	int locateCellRow(Cell *cell);
	int locateCellSubRow(Cell *cell, Row *row);
	void addCell(Cluster *Cluster, Cell *cell, double x_final);
	void addCluster(Cluster *cPrime, Cluster *Cluster);
	Cluster* collapse(Cluster *cluster, Subrow *subrow);
	void searchUpRow(Cell *cell, int &upRowIdx, int &bestRowIdx, int &bestSubrowIdx, double &cBest);
	void searchDownRow(Cell *cell, int &downRowIdx, int &bestRowIdx, int &bestSubrowIdx, double &cBest);
	void testClusterOverlap(Cluster* cluster, Subrow *subrow);
	void addVirtualCell(Cell *cell, Cluster *cluster, double x_final, Subrow *subrow);
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
