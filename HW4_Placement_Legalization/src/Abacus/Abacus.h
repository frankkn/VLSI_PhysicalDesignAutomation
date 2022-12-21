#pragma once
#include "../OutputWriter/OutputWriter.h"
#include "../Module/Module.h"

class Legalizer
{
    Input *input;

    void addCell(Cluster* c, Cell* i, double optimalX);
    void addCluster(Cluster* cPrime, Cluster* c);
    Cluster* collapse(Cluster* c, SubRow* s);

    void divideRow();
    int getRowIdx(Cell const *cell);
    int getSubRowIdx(Row const *row, Cell const *cell);
    void placeRowTrail(int const &rowIdx, int const &subrowIdx,Cell *cell);
    void placeRowFinal(int const &rowIdx, int const &subRowIdx, Cell *cell);
    double calCost(Cell const *cell);
    void determinePosition();
    void abacusProcess();
    void calDisplacement();


public:
    Legalizer(Input *input) : input(input) {}
    ResultWriter *solve();
};
