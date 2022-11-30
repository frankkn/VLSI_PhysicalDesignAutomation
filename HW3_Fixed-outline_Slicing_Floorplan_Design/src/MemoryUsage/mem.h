#pragma once
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class MemoryUsage
{
  private:
    double vm_usage, resident_set;
    void mem_usage(double& vm_usage, double& resident_set);

  public:
    MemoryUsage() {}
    void PrintMemUsage();
};