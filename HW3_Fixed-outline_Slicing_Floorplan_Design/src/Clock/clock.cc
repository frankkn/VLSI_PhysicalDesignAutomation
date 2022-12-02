#include <iostream>
#include <string>
#include "clock.h"

chrono::seconds Clock::getTimeLimit()
{
  return time_limit;
}

chrono::microseconds Clock::getDuration(string const &motion)
{
  auto begin = TimeTable[motion].first;
  auto end = TimeTable[motion].second;
  auto d = end-begin;
  return chrono::duration_cast<chrono::microseconds>(d);
}

void Clock::StartClock(string const &motion)
{
  if(TimeTable.count(motion))
  {
    TimeTable[motion].first = chrono::steady_clock::now();
  }
  else
  {
    pair<chrono::steady_clock::time_point, chrono::steady_clock::time_point> time_pair;
    time_pair.first = time_pair.second = chrono::steady_clock::now();
    TimeTable.emplace(motion, move(time_pair));
  }
}

void Clock::EndClock(string const &motion)
{
  TimeTable[motion].second = chrono::steady_clock::now();
}

void Clock::PrintDuration(string const &motion)
{
  auto d = getDuration(motion).count();
  cout << motion << ": " << d/1000000.0 << "seconds\n";
}

bool Clock::OutOfTime()
{
  auto begin = TimeTable["time_limit"].first;
  auto cur_time = chrono::steady_clock::now();
  auto d = cur_time - begin;
  // Need to debug on line 49
  // return chrono::duration_cast<chrono::microseconds>(d)/1000000.0 > time_limit;
  return chrono::duration_cast<chrono::microseconds>(d).count()/1000000.0 > _time_limit;
}