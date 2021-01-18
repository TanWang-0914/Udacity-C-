#include <string>
#include <stdio.h>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    char cstring[10];
    int num_hour = seconds/3600;
    int num_min = (seconds-num_hour*3600)/60;
    int num_sec = seconds-num_hour*3600-num_min*60;
    sprintf(cstring, "%.2d:%.2d:%.2d", num_hour, num_min, num_sec);
    return cstring;
}