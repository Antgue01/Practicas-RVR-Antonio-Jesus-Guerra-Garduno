#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char **argv)
{

    if (argc != 3){

        std::cerr << "USAGE e1 <hostname> <port>\n";
        return -1;
    }
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; //0 para todos
    int codes = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (codes != 0) {
        std::cerr << gai_strerror(codes) << '\n';
        return -1;
    }
    for (addrinfo *i = res; i != nullptr; i = i->ai_next)
    {
        char *host = new char[NI_MAXHOST];
        char *serv = new char[NI_MAXSERV];
        codes = getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV,
                            NI_NUMERICHOST | NI_NUMERICSERV);
        if (codes != 0 || host == nullptr || serv == nullptr)
        {
            delete host;
            delete serv;
            std::cerr << gai_strerror(codes) << '\n';
            freeaddrinfo(res);
            return -1;
        }
        std::cout << "IP: " << host << '\n';
        std::cout << "Port: " << serv << '\n';
        std::cout << "Family: " << i->ai_family << '\n';
        std::cout << "Type: " << i->ai_socktype << "\n\n";
             delete host;
        delete serv;
    }
    freeaddrinfo(res);
    return 0;
}
