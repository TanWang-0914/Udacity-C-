#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string memkey, kb, val;
  float memtotal, memfree;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> memkey >> val >> kb;
      if (memkey == "MemTotal") memtotal = std::stof(val);
      if (memkey == "MemFree") memfree = std::stof(val);
    }
  }
  return (memtotal-memfree)/memtotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime_long;
  string uptime, idletime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  	std::istringstream linestream(line);
    linestream >> uptime >> idletime;
    uptime_long = std::stol(uptime);
  }
  return uptime_long;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string var;
  long int utime, stime, cutime, cstime;
  vector<string> var_list;
  long total_jiffies;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> var){
      var_list.push_back(var);
    }
    utime = stol(var_list[13]);
    stime = stol(var_list[14]);
    cutime = stol(var_list[15]);
    cstime = stol(var_list[16]);
    total_jiffies = utime + stime + cutime + cstime;
  }
  return total_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_time = CpuUtilization();
  long active_jiffies = stol(cpu_time[CPUStates::kUser_]) + stol(cpu_time[CPUStates::kNice_]) + 
    stol(cpu_time[CPUStates::kSystem_]) + stol(cpu_time[CPUStates::kIRQ_]) +
    stol(cpu_time[CPUStates::kSoftIRQ_]) + stol(cpu_time[CPUStates::kSteal_]);
  return active_jiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_time = CpuUtilization();
  return stol(cpu_time[CPUStates::kIdle_]) + stol(cpu_time[CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string cpu, cpu_time;
  string line;
  vector<string> cpu_utilization;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;

    while (linestream >> cpu_time) {
      cpu_utilization.push_back(cpu_time);
    }
  }
  return cpu_utilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key, val;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> val){
        if (key == "processes"){
          return stoi(val);
        }
      }
    }
  }
  return -1;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key, val;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> val){
        if (key == "procs_running"){
          return stoi(val);
        }
      }
    }
  }
  return -1;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    return line;
  }
  return "-1";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key, val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> val){
        if (key == "VmSize:"){
          return (to_string(stoi(val)/1024));
        }
      }
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key, val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == "Uid:") {
          return val;
        }
      }
    }
  }
  return "-1";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string user, x, uid_lhs;
  string uid_rhs{LinuxParser::Uid(pid)};
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid_lhs) {
        if (uid_lhs == uid_rhs) {
          return user;
        }
      }
    }
  }
  return "-1";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, var;
  vector<string> var_list;
  long int starttime, uptime;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> var){
      var_list.push_back(var);
    }
  }  
  if (var_list.size() >= 22) {
    starttime = std::stol(var_list[21], nullptr, 10)/sysconf(_SC_CLK_TCK);
    uptime = LinuxParser::UpTime() - starttime;
    return uptime;
  }
  return -1;
}