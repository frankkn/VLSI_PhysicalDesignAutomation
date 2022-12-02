#pragma once
#include <chrono>
#include <unordered_map>
using namespace std;

class Clock
{
  private:
    chrono::seconds time_limit;
    unordered_map<string, pair<chrono::steady_clock::time_point, chrono::steady_clock::time_point>> TimeTable;
    chrono::microseconds getDuration(string const &motion);
  public:
    Clock(int time_limit):time_limit(chrono::seconds(time_limit))
    { 
      auto initTime = chrono::steady_clock::now();
      TimeTable.emplace("time_limit", make_pair(initTime, initTime));
    }
    chrono::seconds getTimeLimit();
    void StartClock(string const &motion);
    void EndClock(string const &motion);
    void PrintDuration(string const &motion);
    bool OutOfTime();
};