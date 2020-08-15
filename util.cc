#include "util.h"

#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <stdlib.h>

// Size of Client and Server Buffers. 
const int INTERNAL_BUFFER_LENGTH = 1120;

std::string ConvertFlagToStr(const std::string& flag) {
    int value_index = flag.find("=") + 1; // get after =
    if (value_index == std::string::npos || value_index == 0) { return "ERRORSTRING"; }    
    return flag.substr(value_index);
}

// assuming flags are in the form --port=XX
int ConvertFlagToInt(const std::string& flag) {
    int value_index = flag.find("=") + 1; // get after =
    if (value_index == std::string::npos || value_index == 0) { return -1; }
    
    int flag_value = std::stoi(flag.substr(value_index), nullptr);
    std::cout << "ENTERED VALUE " << flag_value << " FOR FLAG " << flag << std::endl;

    return flag_value;
}

bool ReportError(bool condition, const std::string& error_message) {
    if (condition) { 
        std::cerr << error_message << std::endl; 

        if (errno > 0) {
            std::cerr << "Operation failed with err no: " << errno << std::endl;
        }

        return true;
    }
    return false;
}

struct sockaddr_in GetSockAddr(int port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    return addr;
}

int RandomNumber(int min, int max) {
    return (rand() % max) + min;
}