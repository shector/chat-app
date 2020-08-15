#ifndef util_h
#define util_h

#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

extern const int INTERNAL_BUFFER_LENGTH;

// takes a flag and extract the name.
std::string ConvertFlagToStr(const std::string& flag);

// takes a flag and extracts the name and returns int argument
int ConvertFlagToInt(const std::string& flag);

// Checks for given error condition, print out to standard error if true
// NOTE: In a bigger system exceptions might be more suitable . 
bool ReportError(bool condition, const std::string& error_message);

// Returns a socket address for the given port to use for binding socket.
struct sockaddr_in GetSockAddr(int port);

int RandomNumber(int min, int max);

#endif