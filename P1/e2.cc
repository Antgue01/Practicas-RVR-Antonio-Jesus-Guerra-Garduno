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
        if (recvfrom(socketDescriptor, (char *)&input, sizeof(char), 0, &client, &size) == -1)
        {
            std::cout << "Error receiving data: " << strerror(errno) << '\n';
            delete timeData;
            freeaddrinfo(result);
            return -1;
        }
        switch (input)
        {
        case 't':
            strftime(buffer, bytes, "%R", timeData);
            break;
        case 'd':
        {

            char *y = new char[4];
            char *m = new char[2];
            char *d = new char[2];

            strftime(y, 4, "%Y", timeData);
            strftime(m, 2, "%m", timeData);
            strftime(d, 2, "%d", timeData);

            char *s = new char[11];
            char bar = '/';
            strncat(s, d, 2);
            strncat(s, &bar, 1);
            strncat(s, m, 2);
            strncat(s, &bar, 1);
            strncat(s, y, 4);
            buffer = s;
            buffer[10] = '\0';
        }
        break;
        default:
            std::cout << "Unknown command.\n";
            break;
        }
        bytes = 11;
        sendto(socketDescriptor, buffer, bytes * sizeof(char), 0, &client, size);
        delete timeData;
    }
    freeaddrinfo(result);
    close(socketDescriptor);
    return 0;
}
void fillBuffer(char input)
{
}
