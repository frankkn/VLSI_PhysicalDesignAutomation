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
    void cutSubRow();
    void locateCellCorerow(Node* cell, int &rowIdx);
    void locateCellSubrow(CoreRow* corerow, Node* cell, int &subrowIdx);
    void addCell(Cluster* c, Node* i);
    void addCluster(Cluster* cPrime, Cluster* c);
    void collapse(Cluster* c, SubRow* s);
    void placeTrialRow(Node* cell, int &corerowIdx, int &subrowIdx);
    double determineCost(Node* cell);
    void placeFinalRow(Node* cell, int &bestCorerowIdx, int &bestSubrowIdx);
    void transformPosition();
    void abacusDP();
    OutputWriter* run();
};