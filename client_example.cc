#include "util.h"

#include <iostream>
#include <memory>
#include <string.h>

extern const int INTERNAL_BUFFER_LENGTH;

// Send a message to the server with a timeout for confirmation. 
// FLAGS: --port: port to send message to, --msg: message to send (DOES NOT ALLOW SPACES)
int main(int argc, char** argv) {
    if (ReportError(argc < 3, "incorrect number of args")) { return EXIT_FAILURE; }

    int server_port = ConvertFlagToInt(std::string(argv[1]));
    std::string msg = ConvertFlagToStr(argv[2]);

    if (ReportError(server_port == -1, "Unable to parse flag for port. Please enter flag in form of --port=XXXX")) {
        return EXIT_FAILURE;
    }
    
    if (ReportError(msg == "ERRORCHAR", "Unable to parse flag for msg. Please enter flag in form of msg=foo")) {
        return EXIT_FAILURE;
    }

    // One byte per char so multiple by 8.
    if (strlen(msg.c_str()) * 8 > INTERNAL_BUFFER_LENGTH) {
        std::cerr << "MESSAGE TOO LARGE TO BE SENT. PLEASE SEND SHORTER MESSAGE." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "ENTERED VALUE " << msg << " FOR FLAG " << argv[2] << std::endl;

    // Create Socket to use with IPv4 addresses and default protocol.
    int client;
    if (ReportError((client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1, "Socket open failed")) {
        return EXIT_FAILURE;
    }

    // Set Socket Options turn on address reuse. Any nonzero value is fine.
    int yes_value = 1;

    if (ReportError(setsockopt(client, SOL_SOCKET, SO_REUSEPORT, &yes_value, 
        sizeof(yes_value)) == -1, "setting socket option REUSEPORT failed")) {
        return EXIT_FAILURE;
    }

    // Bind socket to an address.
    struct sockaddr_in server_addr = GetSockAddr(server_port);
    int addrlen = sizeof(server_addr);
    if (ReportError(connect(client, (struct sockaddr *)&server_addr, addrlen) == -1, 
        "failed to connect to server, did you start it?")) {
        return EXIT_FAILURE;
    }   

    int bytes_sent = send(client, msg.c_str(), strlen(msg.c_str()), 0);
    std::cout << bytes_sent << " bytes sent to server" << std::endl;
   
    return 0;
}