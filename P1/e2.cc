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

    size_t bytes = 21;
    char *buffer = new char[bytes];
    char input = ' ';
    while (input != 'q')
    {
        const time_t my_time = time(nullptr);
        struct sockaddr client;
        socklen_t size = sizeof(struct sockaddr);
        tm *timeData = localtime(&my_time);
        if (recvfrom(socketDescriptor, (void *)&input, sizeof(char), 0, &client, &size) == -1)
        {
            std::cout << "Error receiving data: " << strerror(errno) << '\n';
            freeaddrinfo(result);
            return -1;
        }
        switch (input)
        {
        case 't':
        {

            char aux[10];
            int nBytes = strftime(aux, bytes, "%R", timeData);
            aux[nBytes] = '\0';
            buffer = aux;
        }
        break;
        case 'd':
        {
            char t[14];
            int nBytes = strftime(t, sizeof(char) * 13, "%d/%m/%Y", timeData);
            t[nBytes] = '\0';
            buffer = t;
        }
        break;
        case 'q':
            std::cout << "shutting down...\n";
            break;
        default:
            std::cout << "Unknown command.\n";
        }
        bytes = 11;
        sendto(socketDescriptor, buffer, bytes * sizeof(char), 0, &client, size);
    }
    close(socketDescriptor);
    freeaddrinfo(result);
    delete buffer;
    return 0;
}
void fillBuffer(char input)
{
}
