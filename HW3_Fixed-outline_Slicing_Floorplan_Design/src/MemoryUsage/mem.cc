#include "mem.h"
using namespace std;

void MemoryUsage::mem_usage(double& vm_usage, double& resident_set)
{
  vm_usage = 0.0;
  resident_set = 0.0;

  //get info from proc directory
  //create some variables to get info
  ifstream stat_stream("/proc/self/stat", ios_base::in); 

  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;
  unsigned long vsize;
  long rss;
  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
  >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
  >> utime >> stime >> cutime >> cstime >> priority >> nice
  >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest
  stat_stream.close();
  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // for x86-64 is configure to use 2MB pages
  vm_usage = vsize / 1024.0;
  resident_set = rss * page_size_kb;
}

void MemoryUsage::PrintMemUsage()
{
  mem_usage(vm_usage, resident_set);
  cout << "Virtual Memory: " << vm_usage << "kB\n" 
       << "Resident set size: " << resident_set << "kB\n";
}