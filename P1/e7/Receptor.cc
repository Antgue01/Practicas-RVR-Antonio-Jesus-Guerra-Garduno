#include "Receptor.h"

void Receptor::Receive(int bytes)
{
    char *buffer = new char[bytes];
    const char *exitInput = "Q";
    while (buffer != "")
    {

        int nBytes = 0;
        nBytes = recv(_sc, (void *)buffer, sizeof(char) * (bytes - 1), 0);
        if (nBytes == -1)
        {
            std::cout << "Error receive: " << strerror(errno) << '\n';
            close(_sc);
            delete buffer;
            return;
        }
        if (nBytes == 0 || strncmp(buffer, exitInput, nBytes) == 0 || (buffer[0] == 'Q' && buffer[1] == '\n'))
            break;
        buffer[nBytes] = '\0';
        for (int i = 0; i < 10; i++)
        {
            std::cout<<i<<'\n';
            sleep(1);
        }
        
        nBytes = send(_sc, (void *)buffer, sizeof(char) * nBytes, 0);
        if (nBytes == -1)
        {
            std::cout << "Error send: " << strerror(errno) << '\n';
            close(_sc);
            delete buffer;
            return;
        }
    }
    close(_sc);
    delete buffer;
}
