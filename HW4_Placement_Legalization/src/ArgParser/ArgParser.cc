#include "ArgParser.h"
#include <unordered_map>
#include <iostream>
using namespace std;

void Parser::readAux(string const & auxPath, string &nodePath, string &plPath, string &sclPath)
{
  ifstream fin(auxPath);
  string s;
  while(getline(fin, s)) 
  {
    stringstream ss(s);
    string begin_word, colon;
    while(ss >> begin_word) 
    {
      if(begin_word == "RowBasedPlacement") 
      {
        ss >> colon >> nodePath >> plPath >> sclPath;
      }
      else 
      {
        ss >> colon >> maxDisplacement;
      }
    }
  }
}

void Parser::readNode(string const &nodePath)
{
  ifstream fin(nodePath);
  string s;
  int nodeCnt, terminalCnt;
  while(getline(fin, s)) 
  {
    if(s.empty()) break; // Otherwise, read until EOF
    stringstream ss(s);
    string begin_word, colon;
    while(ss >> begin_word) 
    {
      if(begin_word == "NumNodes") 
      {
        ss >> colon >> nodeCnt;
      }
      else 
      {
        ss >> colon >> terminalCnt;
      }
    }
  }

  int cellCnt = nodeCnt - terminalCnt;

  for(int i = 0; i < cellCnt; ++i) 
  {
    getline(fin, s);
    stringstream ss(s);
    string cell_name; 
    int width, height;
    ss >> cell_name >> width >> height;
    Cell* cell = new Cell(cell_name, width, height);
    cellList.emplace_back(cell);
    NodeTable.emplace(cell_name, cell);
  }
  for(int i = 0; i < terminalCnt; ++i) 
  {
    getline(fin, s);
    stringstream ss(s);
    string terminal_name, tmp;
    int width, height;
    ss >> terminal_name >> width >> height >> tmp;
    Cell* terminal = new Cell(terminal_name, width, height);
    terminalList.emplace_back(terminal);
    NodeTable.emplace(terminal_name, terminal);
  }
}

void Parser::readPl(string const &plPath)
{
  ifstream fin(plPath);
  string s;
  for(int i = 0; i < cellList.size(); ++i) 
  {
    getline(fin, s);
    stringstream ss(s);
    string cell_name, colon, N;
    double cell_x, cell_y;
    ss >> cell_name >>  cell_x >> cell_y >> colon >> N;
    NodeTable.at(cell_name)->x = cell_x;
    NodeTable.at(cell_name)->y = cell_y;
  }
  for(int i = 0; i < terminalList.size(); ++i) 
  {
    getline(fin, s);
    stringstream ss(s);
    string terminal_name, colon, N, F;
    double terminal_x, terminal_y;
    ss >> terminal_name >> terminal_x >> terminal_y >> colon >> N >> F;
    NodeTable.at(terminal_name)->x = terminal_x;
    NodeTable.at(terminal_name)->y = terminal_y;
  }
}

void Parser::readScl(string const &sclPath)
{
	ifstream fin(sclPath);
  string s, tmp, colon;
  int rowNum = 0;
  getline(fin, s);
  stringstream ss(s);
  ss >> tmp >> colon >> rowNum;
  // cout << rowNum << endl;
  getline(fin, s);
  for(int i = 0; i < rowNum; ++i) 
  {
    int y = 0, height = 0, siteWidth = 0, siteNum = 0, x = 0;
    for(int j = 0; j < 7; ++j) 
    {
      getline(fin, s);
      // cout << j << "!" << s << endl;
      if(j == 0 || j == 6) continue;
      else 
      {
        stringstream ss(s);
        string begin_word, colon;
        ss >> begin_word;
        if(begin_word == "Coordinate")
        {
          ss >> colon >> y;
        }
        else if(begin_word == "Height")
        {
          ss >> colon >> height;
        }
        else if(begin_word == "Sitewidth")
        {
          ss >> colon >> siteWidth;
        }
        else if(begin_word == "NumSites")
        {
          ss >> colon >> siteNum;
        }
        // begin_word == "SubrowOrigin"
        else 
        { 
          ss >> colon >> x;
        }
      }
    }
    auto row = new Row(siteWidth, height, y);
    auto subrow = new SubRow(x, x + siteWidth * siteNum);
    row->subRows.emplace_back(subrow);
    rowList.emplace_back(row);
  }
}

Input *Parser::parse(char argv[])
{
  string auxPath = string(argv);
  string nodePath, plPath, sclPath;
  readAux(auxPath, nodePath, plPath, sclPath);
  string prefix = auxPath.erase(auxPath.find_last_of('/')+1, auxPath.size()-1);
  readNode(prefix + nodePath);
  readPl(prefix + plPath);
  readScl(prefix + sclPath);
	return new Input(this->maxDisplacement, this->cellList, this->terminalList, this->rowList);
}
