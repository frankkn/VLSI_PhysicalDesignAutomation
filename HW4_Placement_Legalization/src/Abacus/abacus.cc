#include "./abacus.h"
using namespace std;

void AbacusLegalizer::cutSubRow() {
  sort(input->terminals.begin(), input->terminals.end(), [&](auto &a, auto &b){ return a->x < b->x; });
  for(auto &coreRow: block) {
    auto curSubRow = coreRow->subrows.back();
    for(auto &terminal: terminals) {
      if(terminal->y >= curSubRow->y+curSubRow->height || terminal->y + terminal->height <= curSubRow->y) { continue; }
      int tLeft = terminal->x, tRight = terminal->x + terminal->width;
      if(curSubRow->leftX >= tLeft) {
        if(curSubRow->rightX > tRight) {
          curSubRow->leftX = tRight;
        }else {
          delete curSubRow;
          coreRow->subrows.pop_back();
        }
      }else {
        if(curSubRow->rightX < tRight) {
          curSubRow->rightX = tLeft;
        }else {
          curSubRow->rightX = tLeft;
          coreRow->subrows.emplace_back(new SubRow(tRight, curSubRow->rightX));
        }
      }
    }
  }
}