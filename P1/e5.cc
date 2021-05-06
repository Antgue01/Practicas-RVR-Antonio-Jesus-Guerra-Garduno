#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

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

    const struct sockaddr server = *result->ai_addr;
    socklen_t size = sizeof(struct sockaddr);
    returnCode = connect(socketDescriptor, &server, size);
    if (socketDescriptor == -1)
    {
        std::cout << "Error creating socket: " << strerror(errno) << '\n';
        freeaddrinfo(result);
        close(socketDescriptor);
        return -1;
    }
    char *host = new char[NI_MAXHOST];
    char *serv = new char[NI_MAXSERV];
    returnCode = getnameinfo(&server, sizeof(struct sockaddr), host, NI_MAXHOST, serv, NI_MAXSERV, 0);

    if (returnCode != 0)
    {
        std::cout << "error getting name info" << gai_strerror(returnCode);
    }
    std::cout << "IP: " << host << '\n';
    std::cout << "Port: " << serv << '\n';
    delete host;
    delete serv;

    size_t bytes = 256;
    char *buffer = new char[bytes];
    int nBytes = 1;
    while (nBytes != 0)
    {
        memset((void *)buffer, 0, bytes);
        nBytes = recv(socketDescriptor, (void *)buffer, bytes - 1, 0);
        if (nBytes == -1)
        {
            std::cout << "Error receive: " << strerror(errno) << '\n';
            freeaddrinfo(result);
            close(socketDescriptor);
            close(socketDescriptor);
            delete buffer;
            return -1;
        }
        buffer[nBytes] = '\0';
        nBytes = send(socketDescriptor, (void *)buffer, nBytes, 0);
        if (nBytes == -1)
        {
            std::cout << "Error receive: " << strerror(errno) << '\n';
            freeaddrinfo(result);
            close(socketDescriptor);
            delete buffer;
            return -1;
        }
    }

    close(socketDescriptor);
    freeaddrinfo(result);
    delete buffer;
    std::cout << "shutting down...\n";
    return 0;
}
