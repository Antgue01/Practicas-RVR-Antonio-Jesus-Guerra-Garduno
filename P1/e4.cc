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

    bind(socketDescriptor, (struct sockaddr *)result->ai_addr, result->ai_addrlen);

    returnCode = listen(socketDescriptor, 2);
    if (returnCode == -1)
    {
        std::cout << "Error on listen: " << strerror(errno) << '\n';
        freeaddrinfo(result);
        return -1;
    }
    struct sockaddr client;
    socklen_t size = sizeof(struct sockaddr);
    int clientSocket = accept(socketDescriptor, &client, &size);

    if (clientSocket < 0)
    {
        std::cout << "Error on accept: " << strerror(errno) << '\n';
        freeaddrinfo(result);
        close(socketDescriptor);
        return -1;
    }

    char *host = new char[NI_MAXHOST];
    char *serv = new char[NI_MAXSERV];
    returnCode = getnameinfo(&client, sizeof(struct sockaddr), host, NI_MAXHOST, serv, NI_MAXSERV, 0);

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
    const char *exitInput = "Q";

    while (true)
    {
        nBytes = recv(clientSocket, (void *)buffer, sizeof(char) * (bytes - 1), 0);
        if (nBytes == -1)
        {
            std::cout << "Error receive: " << strerror(errno) << '\n';
            freeaddrinfo(result);
            close(socketDescriptor);
            close(clientSocket);
            delete buffer;
            return -1;
        }
        if (nBytes == 0 || strncmp(buffer, exitInput, nBytes) == 0 || (buffer[0] == 'Q' && buffer[1] == '\n'))
            break;
        buffer[nBytes] = '\0';
        nBytes = send(clientSocket, (void *)buffer, sizeof(char) * nBytes, 0);
        if (nBytes == -1)
        {
            std::cout << "Error send: " << strerror(errno) << '\n';
            freeaddrinfo(result);
            close(socketDescriptor);
            close(clientSocket);
            delete buffer;
            return -1;
        }
    }

    close(socketDescriptor);
    close(clientSocket);
    freeaddrinfo(result);
    delete buffer;
    std::cout << "shutting down...\n";
    return 0;
}
