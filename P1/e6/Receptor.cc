#include "Receptor.h"

void Receptor::Receive(char *buffer, int bytes)
{
    int nBytes = 0;
    char input = ' ';
    while (input != 'q')
    {
        const time_t my_time = time(nullptr);
        struct sockaddr client;
        socklen_t size = sizeof(struct sockaddr);
        tm *timeData = localtime(&my_time);
        _mutex.lock();
        if (recvfrom(_sc, (void *)&input, sizeof(char), 0, &client, &size) == -1)
        {
            std::cout << "Error receiving data: " << strerror(errno) << '\n';
            return;
        }
        char copy = input;
        char *bufferAux = new char[bytes];
        _mutex.unlock();
        switch (copy)
        {
        case 't':
            nBytes = strftime(bufferAux, sizeof(char) * bytes, "%R", timeData);
            bufferAux[nBytes] = '\n';
            bufferAux[nBytes + 1] = '\0';

            break;
        case 'd':

            nBytes = strftime(bufferAux, sizeof(char) * bytes, "%d/%m/%Y", timeData);
            bufferAux[nBytes] = '\n';
            bufferAux[nBytes + 1] = '\0';

            break;
        case 'q':
            std::cout << "shutting down...\n";
            bufferAux[0] = '\n';
            bufferAux[1] = '\0';
            return;
        default:
            bufferAux[0] = '\n';
            bufferAux[1] = '\0';
            std::cout << "Unknown command.\n";
            break;
        }
        for (size_t i = 0; i < 10; i++)
        {
            std::cout << i << std::endl;
            sleep(1);
        }

        _mutex.lock();
        buffer = bufferAux;
        sendto(_sc, buffer, nBytes * sizeof(char) + 1, 0, &client, size);
        _mutex.unlock();
        std::cout << std::this_thread::get_id() << '\n';
        char *host = new char[NI_MAXHOST];
        char *port = new char[NI_MAXSERV];
        int returnCode = getnameinfo(&client, size, host, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        if (returnCode != 0)
        {
            delete host;
            delete port;
            std::cerr << gai_strerror(returnCode) << '\n';
            return;
        }
        std::cout << "host: " << host << '\n'
                  << "port: " << port << '\n';
        delete host;
        delete port;
    }
}