#ifndef RECEPTOR_H
#define RECEPTOR_H
#include <thread>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <iostream>
#include <sys/socket.h>
#include <mutex>
#include <netdb.h>
class Receptor
{
public:
    Receptor(int socketDescriptor) : _sc(socketDescriptor), _mutex() {}
    Receptor(const Receptor &old_obj) { _sc = old_obj._sc; }
    void Receive(int bytes);

private:
    int _sc;
    std::mutex _mutex;
};
#endif