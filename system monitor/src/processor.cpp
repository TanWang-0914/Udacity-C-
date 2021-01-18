#include<iostream>

#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::stoi;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float utilization{0};
  long idle_jiffies = LinuxParser::IdleJiffies();
  long active_jiffies = LinuxParser::ActiveJiffies();

  long delta_active_jiffies = active_jiffies - old_active_jiffies_;
  long delta_idle_jiffes = idle_jiffies - old_idle_jiffies_;
  long total_jiffies = delta_active_jiffies + delta_idle_jiffes;

  utilization = static_cast<float>(delta_active_jiffies)/total_jiffies;
  old_active_jiffies_ = active_jiffies;
  old_idle_jiffies_ = idle_jiffies;
  
  return utilization;
}