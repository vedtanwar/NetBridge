#ifndef REACTOR_H
#define REACTOR_H

#include <cstdint>
#include <unordered_map>
#include<thread>

class IEventHandler;

class Reactor
{
private:
    /* data */
    std::thread eventLoopThread;
    int m_epollFd {-1};
    std::unordered_map<int, IEventHandler*> m_handler;
    
    
public:
    Reactor(/* args */);
    ~Reactor();

    bool registerFd(int fd, uint32_t events, IEventHandler* handler);
    void unregisteredFd(int fd);
    void run();

};

#endif // REACTOR_H
