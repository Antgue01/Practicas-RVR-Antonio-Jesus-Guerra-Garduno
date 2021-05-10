#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <vector>
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
    hints.ai_socktype = SOCK_STREAM;

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

    returnCode = listen(socketDescriptor, NUM_THREADS);
    if (returnCode == -1)
    {
        std::cout << "Error on listen: " << strerror(errno) << '\n';
        freeaddrinfo(result);
        return -1;
    }

    size_t bytes = 256;
    std::vector<Receptor *> receptors;
    char input = ' ';
    while (input != 'Q')
    {
        std::cin >> input;
        struct sockaddr client;
        socklen_t size = sizeof(struct sockaddr);
        int clientSocket = accept(socketDescriptor, &client, &size);
        if (clientSocket < 0)
        {
            std::cout << "Error on accept: " << strerror(errno) << '\n';
            //Cuándo se cierra el descriptor del cliente y se borra la memoria dinámica?
        }
        else
        {
            char *host = new char[NI_MAXHOST];
            char *serv = new char[NI_MAXSERV];
            returnCode = getnameinfo(&client, sizeof(struct sockaddr), host, NI_MAXHOST, serv, NI_MAXSERV, 0);

            if (returnCode != 0)
            {
                std::cout << "error getting name info" << gai_strerror(returnCode);
            }
            else
            {
                std::cout << "IP: " << host << '\n';
                std::cout << "Port: " << serv << '\n';
            }
            delete host;
            delete serv;

            Receptor *rec = new Receptor(clientSocket);
            receptors.push_back(rec);
            std::thread(&Receptor::Receive, rec, bytes).detach();
        }
    }
    for (Receptor *it : receptors)
    {
        delete it;
        it = nullptr;
    }
    std::cout << "Shutting down...\n";
    receptors.clear();
    close(socketDescriptor);
    freeaddrinfo(result);
    delete buffer;
    return 0;
}
void a() {}