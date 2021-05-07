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
    int nBytes = 0;
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
            delete buffer;
            return -1;
        }
        switch (input)
        {
        case 't':

            nBytes = strftime(buffer, sizeof(char) * bytes, "%R", timeData);
            buffer[nBytes] = '\0';

            break;
        case 'd':

            nBytes = strftime(buffer, sizeof(char) * bytes, "%d/%m/%Y", timeData);
            buffer[nBytes] = '\0';

            break;
        case 'q':
            std::cout << "shutting down...\n";
            buffer[0] = '\0';
            break;
        default:
            buffer[0] = '\0';
            std::cout << "Unknown command.\n";
            break;
        }
        sendto(socketDescriptor, buffer, nBytes * sizeof(char), 0, &client, size);
        char *host = new char[NI_MAXHOST];
        char *port = new char[NI_MAXSERV];
        returnCode = getnameinfo(&client, size, host, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        if (returnCode != 0)
        {
            delete host;
            delete port;
            delete buffer;
            std::cerr << gai_strerror(returnCode) << '\n';
            freeaddrinfo(result);
            return -1;
        }
        std::cout << "host: " << host << '\n'
                  << "port: " << port << '\n';
        delete host;
        delete port;
    }

    close(socketDescriptor);
    freeaddrinfo(result);
    delete buffer;
    return 0;
}
