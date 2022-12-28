#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../Module/Module.h"
using namespace std;

class Parser
{
	int maxDisplacement;
	vector<Cell *> cellList, terminalList;
	vector<Row *> rowList;
	unordered_map<string, Cell*> NodeTable;

	void readAux(string const & auxPath, string &nodePath, string &plPath, string &sclPath);
	void readNode(string const & nodePath);
	void readPl(string const & plPath);
	void readScl(string const & filePath);

public:
	Parser() : maxDisplacement(0) {}
	Input *parse(char argv[]);
};
