#include <fstream>
#include "OutputWriter.h"
using namespace std;

void ResultWriter::write(char *argv[])
{
    string auxFilePath(argv[1]);
    auto filename = auxFilePath.substr(auxFilePath.find_last_of('/') + 1,
                                       auxFilePath.find_last_of('.') - auxFilePath.find_last_of('/') - 1);
    ofstream fout("../output/" + filename + ".result");
    for (auto const &cell : input->cellList)
    {
        fout << cell->name << ' '
             << static_cast<int>(cell->optimalX) << ' '
             << static_cast<int>(cell->optimalY) << " : N\n";
    }
    for (auto const &terminal : input->terminalList)
    {
        fout << terminal->name << ' '
             << static_cast<int>(terminal->x) << ' '
             << static_cast<int>(terminal->y) << " : N /FIXED\n";
    }
}
