#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <unistd.h>
#include "Receptor.h"

#define NUM_THREADS 3
int main(int argc, char **argv)
{

    if (argc != 3)
    {

        std::cerr << "USAGE e2 <host> <port>\n";
        return -1;
    }
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int returnCode = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (returnCode != 0)
    {
        std::cout << "Error getting address info: " << gai_strerror(returnCode) << '\n';
        return -1;
    }
    int socketDescriptor = socket(result->ai_family, result->ai_socktype, 0);
    if (socketDescriptor == -1)
    {
        std::cout << "Error creating socket: " << strerror(errno) << '\n';
        freeaddrinfo(result);
        return -1;
    }
    bind(socketDescriptor, (struct sockaddr *)result->ai_addr, result->ai_addrlen);
    std::thread threads[NUM_THREADS];

    size_t bytes = 21;
    char *buffer = new char[bytes];
    char input = ' ';
    Receptor *receptors[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
    {
        receptors[i] = new Receptor(socketDescriptor);
        threads[i] = std::thread( &Receptor::Receive,*receptors[i], buffer,bytes);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads[i].join();
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        delete receptors[i];
    }

    

    close(socketDescriptor);
    freeaddrinfo(result);
    delete buffer;
    return 0;
}
void a() {}