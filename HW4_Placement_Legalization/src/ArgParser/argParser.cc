#include <iostream>
#include <fstream>
#include <sstream>
#include "argParser.h"
using namespace std;

void ArgParser::ReadAux(string const & auxPath, string &nodePath, string &plPath, string &sclPath) {
  ifstream fin(auxPath);
  string s;
  while(getline(fin, s)) {
    stringstream ss(s);
    string begin_word, colon;
    while(ss >> begin_word) {
      if(begin_word == "RowBasedPlacement") {
        ss >> colon >> nodePath >> plPath >> sclPath;
      }else {
        ss >> colon >> maxDisplacement;
      }
    }
  }
}

void ArgParser::ReadNode(string const &nodePath) {
  ifstream fin(nodePath);
  string s;
  int nodeCnt, terminalCnt;
  while(getline(fin, s)) {
    if(s.empty()) break;
    stringstream ss(s);
    string begin_word, colon;
    while(ss >> begin_word) {
      if(begin_word == "NumNodes") {
        ss >> colon >> nodeCnt;
      }else {
        ss >> colon >> terminalCnt;
      }
    }
  }

  int cellCnt = nodeCnt - terminalCnt;

  for(int i = 0; i < cellCnt; ++i) {
    getline(fin, s);
    stringstream ss(s);
    string cell_name; 
    int width, height;
    ss >> cell_name >> width >> height;
    Node* cell = new Node(cell_name, width, height);
    cells.emplace_back(cell);
    NodeTable.emplace(cell_name, cell);
  }
  for(int i = 0; i < terminalCnt; ++i) {
    getline(fin, s);
    stringstream ss(s);
    string terminal_name, width, height, tmp;
    ss >> terminal_name >> width >> height >> tmp;
    Node* terminal = new Node(terminal_name, stoi(width), stoi(height));
    terminals.emplace_back(terminal);
    NodeTable.emplace(terminal_name, terminal);
  }
  // cout << NodeTable.size();
}

void ArgParser::ReadPl(string const &plPath) {

}

void ArgParser::ReadScl(string const &sclPath) {

}

void ArgParser::ReadFile(char argv[]) {
  string auxPath = string(argv);
  string nodePath, plPath, sclPath;
  ReadAux(auxPath, nodePath, plPath, sclPath);
  string prefix = auxPath.erase(auxPath.find_last_of('/')+1, auxPath.size()-1);
  ReadNode(prefix + nodePath);
  // ReadPl(prefix + plPath);
  // ReadScl(prefix + sclPath);

  // return new Input(maxDisplacement, cells, terminals, blocks);
}