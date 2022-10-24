#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Net;
class Cell
{
  public:
    string name;
    int size_a, size_b;
    int set;
    int gain;
    bool locked;
    vector<Net*> nets;

    Cell(string name, int size_a, int size_b):name(name), size_a(size_a), size_b(size_b), set(0), locked(false) {}
};

class Net
{
  public:
    string name;
    vector<Cell*> cells; 
    pair<int, int> setCnt;

    Net(string name):name(name), setCnt(make_pair(0,0)) {}

};

class BucketList
{
  public:
    int size;
    unordered_map<int, vector<Cell*>> bucketlist;

    BucketList():size(0) {}
};
