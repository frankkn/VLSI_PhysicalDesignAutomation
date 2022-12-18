#pragma once
#include "../Module/module.h"
#include "../OutputWriter/outputwriter.h"
using namespace std;

class AbacusLegalizer 
{
  private:
    Input* input;
    
    // void cutSubRow();

  public:
    AbacusLegalizer(Input* input):input(input) {}

    // void divideRow();
    // int getRowIdx(Node *cell);
    // int getSubRowIdx(CoreRow *row, Node *cell);
    // int placeRowTrail(int &rowIdx, Node *cell);
    // void placeRowFinal(int &rowIdx, int &subRowIdx, Node *cell);
    // double calCost(Node *cell);
    // void abacusProcess();
    // OutputWriter *solve();

    void cutSubRow();
    void locateCellCorerow(Node* cell, int &rowIdx);
    void locateCellSubrow(CoreRow* corerow, Node* cell, int &subrowIdx);
    void addCell(Cluster* c, Node* i);
    void addCluster(Cluster* cPrime, Cluster* c);
    void collapse(Cluster* c, SubRow* s);
    void placeTrialRow1(Node* cell, int &corerowIdx, int &subrowIdx);
    void placeTrialRow2(Node* cell, int &corerowIdx, int &subrowIdx);
    double determineCost(Node* cell);
    void placeFinalRow1(Node* cell, int &bestCorerowIdx, int &bestSubrowIdx);
    void placeFinalRow2(Node* cell, int &bestCorerowIdx, int &bestSubrowIdx);
    void transformPosition();
    void abacusDP();
    OutputWriter* run();
};