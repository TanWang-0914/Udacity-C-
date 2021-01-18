#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
    pid_ = pid;
    command_ = (LinuxParser::Command(pid_));
    cpu_util_ = Process::CpuUtilization();
    user_ = (LinuxParser::User(pid));
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    float totaltime = LinuxParser::ActiveJiffies(Pid())/sysconf(_SC_CLK_TCK);
    float seconds_active = LinuxParser::UpTime(pid_);
    float cpu_usage = totaltime / seconds_active;
    return cpu_usage;
}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return cpu_util_ > a.cpu_util_;
}