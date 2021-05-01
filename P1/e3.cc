#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char **argv)
{

    if (argc != 4)
    {

        std::cerr << "USAGE e2 <host> <port> t or d\n";
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

    size_t bytes = 40;
    char input = argv[3][0];
    //rellenamos la información del cliente y mandamos el mensaje
    struct sockaddr client;
    socklen_t size = sizeof(struct sockaddr);
    returnCode = sendto(socketDescriptor, (void *)&input, sizeof(char), 0, result->ai_addr, size);
    if (returnCode == -1)
    {
        std::cout << "Error sending message: " << strerror(errno) << '\n';
        freeaddrinfo(result);
        return -1;
    }
    char *buffer = new char[bytes];

    returnCode = recvfrom(socketDescriptor, (void *)buffer, sizeof(char) * bytes, 0, &client, &size);
    if (returnCode == -1)
    {
        std::cout << "Error receiving message: " << strerror(errno) << '\n';
        freeaddrinfo(result);
        return -1;
    }
    std::cout << buffer << '\n';
    close(socketDescriptor);
    freeaddrinfo(result);
    delete buffer;

    return 0;
}
