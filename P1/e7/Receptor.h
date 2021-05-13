#ifndef RECEPTOR_H
#define RECEPTOR_H
#include <thread>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <iostream>
#include <sys/socket.h>
#include <mutex>
#include <condition_variable>
#include <netdb.h>
class Receptor
{
public:
    Receptor(int socketDescriptor, std::mutex *mu, int *clients, std::condition_variable *cond, int numThreads) : 
    _sc(socketDescriptor), _mutex(), main_mutex(mu), numClients(clients), condi(cond), threads(numThreads) {}
    Receptor(const Receptor &old_obj)
    {
        _sc = old_obj._sc;
        main_mutex = old_obj.main_mutex;
        numClients = old_obj.numClients;
        condi = old_obj.condi;
        threads = old_obj.threads;
    }
    void Receive(int bytes);

private:
    int _sc;
    std::mutex _mutex;
    std::mutex *main_mutex;
    std::condition_variable *condi;
    int *numClients;
    int threads;
};
#endif