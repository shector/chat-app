// Start a server on a given port and simply print any message received. 
// Socket Reference: https://pubs.opengroup.org/onlinepubs/007908799/xns/syssocket.h.html
// Standard Socket example: https://www.geeksforgeeks.org/socket-programming-cc/

#include "ctpl_stl.h"
#include "util.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <sys/socket.h>
#include <string>
#include <time.h>
#include <vector>
#include <unistd.h>

extern const int INTERNAL_BUFFER_LENGTH;
// TODO: add some sort of performance testing to find best number of workers for my machine.
const int THREAD_POOL_WORKER_COUNT = 100; // do not block on multiple requests
std::shared_timed_mutex mutex;

// Each thread will have its own buffer. Accesses will be concurrent and require reader lock. 
std::vector<std::unique_ptr<int[]>> CreateThreadBuffers(int size) {
    const std::lock_guard<std::shared_timed_mutex> lock(mutex);
    std::vector<std::unique_ptr<int[]>> buffers;
    buffers.reserve(size);

    for (int i = 0; i < size; i++) {
        // transfer ownership and ensure lifetime of object is tied to lifetime of buffer. 
        buffers.push_back(std::unique_ptr<int[]>(new int[INTERNAL_BUFFER_LENGTH]));
    }
    
    return buffers;
}
            
// FLAGS: --port: port to run on
int main(int argc, char** argv) {
    if (ReportError(argc < 2, "incorrect number of args")) { return EXIT_FAILURE; }
    
    int port = ConvertFlagToInt(std::string(argv[1]));
    if (ReportError(port == -1, "Unable to parse flag. Please enter flag in form of --port=XXXX")) {
        return EXIT_FAILURE;
    }

    // Create Socket to use with IPv4 addresses and default protocol.
    int server;
    if (ReportError((server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1, "Socket open failed")) {
        return EXIT_FAILURE;
    }

    // Set Socket Options turn on address and port reuse. Any nonzero value is fine.
    int yes_value = 1;

    if (ReportError(setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &yes_value, 
        sizeof(yes_value)) == -1, "setting socket option REUSEPORT failed")) {
        return EXIT_FAILURE;
    }

    if (ReportError(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes_value, 
        sizeof(yes_value)) == -1, "setting socket option REUSEADDR failed")) {
        return EXIT_FAILURE;
    }

    // Bind socket to an address.
    struct sockaddr_in addr = GetSockAddr(port);
    int addrlen = sizeof(addr);
    if (ReportError(bind(server, (struct sockaddr *)&addr, addrlen) == -1, 
        "failed to bind port to socket, try a higher number!")) {
        return EXIT_FAILURE;
    }

    // Listen on the port for messages and allow a queue of 10 outstanding. 
    if (ReportError(listen(server, 10) == -1, "failed to listen on socket")) {
        return EXIT_FAILURE;
    }

    std::cout << "Server is now listening on PORT " << port << std::endl;

    // NOTE: Thread pool implementation is borrowed under Apache License from
    // https://github.com/vit-vit/CTPL/blob/master/ctpl_stl.h
    ctpl::thread_pool thread_pool(THREAD_POOL_WORKER_COUNT);

    // NOTE: THREAD BUFFERS IS PROTECTED BY SHARED MUTEX. 
    // Containers allow multiple readers by default so this is not neccessary. 
    std::vector<std::unique_ptr<int[]>> thread_buffers = CreateThreadBuffers(THREAD_POOL_WORKER_COUNT);
    
    srand(time(NULL)); // set random seed. 
    std::string response = "Got your message thanks!";
    while (true) { 
        // Main thread acts as listener to accept new connections from clients.
        int client;
        if (ReportError((client = accept(server, (struct sockaddr *) &addr, 
            (socklen_t*) &addrlen)) == -1, "Failed to open connection with client")) {
            return EXIT_FAILURE;
        }

        // For now just handle the request in a new thread by printing it. 
        thread_pool.push([client, &response, &thread_buffers] (int thread_id) {
            // random sleep to simulate doing work and show async behavior.
            usleep(RandomNumber(1,7) * 1000000); 

            mutex.lock_shared();
            auto local_buffer = thread_buffers[thread_id].get();
            mutex.unlock_shared();


            int bytes_read = 0;
            // buffer length is shared by client and server so buffer will not overflow but be cut off instead. 
            if ((bytes_read = recv(client, local_buffer, INTERNAL_BUFFER_LENGTH, 0)) > 0) {
                std::cout << "Message received: " << std::string((char*)local_buffer, bytes_read) << std::endl;
                std::cout << "Connection handled by thread " << thread_id << std::endl;
                std::cout << "Socket FD: " << client << std::endl;
            }
            ReportError(bytes_read == -1, "read fail");

            // Can send response if client is waiting for one. See client_example.cc. 
            // send(client, response.c_str(), strlen(response.c_str()), 0);
            // std::cout << "Sent Response" << std::endl;
            close(client);
            memset(local_buffer, 0, INTERNAL_BUFFER_LENGTH); // flush for reuse.

        });
    }

    return 0;
} 