#include <algorithm>
#include <iostream>
#include "./abacus.h"
using namespace std;

void AbacusLegalizer::cutSubRow() {
  sort(input->terminals.begin(), input->terminals.end(), [&](auto &a, auto &b){ return a->x < b->x; });
  for(auto &coreRow: input->block) {
    for(auto &terminal: input->terminals) {
      auto curSubRow = coreRow->subrows.back();
      if(terminal->y > coreRow->y || coreRow->y >= terminal->y + terminal->height) { continue; }
      int tLeftX = terminal->x, tRightX = terminal->x + terminal->width;
      if(curSubRow->leftX >= tLeftX) {
        if(curSubRow->rightX > tRightX) {
          curSubRow->leftX = tRightX;
        }else {
          delete curSubRow;
          coreRow->subrows.pop_back();
        }
      }else {
        if(curSubRow->rightX > tRightX) {
          coreRow->subrows.emplace_back(new SubRow(tRightX, curSubRow->rightX));
          curSubRow->rightX = tLeftX;
        }else {
          curSubRow->rightX = tLeftX;
        }
      }
    }
  }
  int cnt = 0;
  for(auto& coreRow: input->block) {
    for(auto& subRow:coreRow->subrows) {
      ++cnt;
    }
  }
  cout << "cnt = " << cnt << endl;
}