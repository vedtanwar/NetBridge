#include "Reactor.h"
#include "IEventHandler.h"
#include "log.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

Reactor::Reactor()
{
    m_epollFd = epoll_create1(0);

    if (m_epollFd == -1)
    {
        Logger::error("Failed to create epoll instance: {}", std::strerror(errno));
        exit(EXIT_FAILURE);
    }
}

Reactor::~Reactor()
{
    if (m_epollFd != -1)
    {
        close(m_epollFd);
    }

    if (eventLoopThread.joinable())
    {
        eventLoopThread.join();
    }
}

bool Reactor::registerFd(int fd, uint32_t events, IEventHandler *handler)
{

    epoll_event ev{};

    ev.events = events;
    ev.data.fd = fd;

    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        Logger::error("Failed to add fd {} to epoll: {}", fd, std::strerror(errno));
        return false;
    }

    m_handler[fd] = handler;

    return true;
}

void Reactor::unregisteredFd(int fd)
{

    if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, nullptr) == -1)
    {
        Logger::error("Failed to delete fd {} from epoll: {} ", fd, std::strerror(errno));
    }

    m_handler.erase(fd);
}

void Reactor::run()
{
    eventLoopThread = std::thread([this]()
                                  {

                                    constexpr int MAX_EVENTS = 64;

                                    epoll_event events[MAX_EVENTS];

                                    while (true)
                                    {

                                        int ready = epoll_wait(m_epollFd, events, MAX_EVENTS, -1);

                                        if (ready == -1)
                                        {

                                            if (errno == EINTR)
                                            {
                                                continue;
                                            }

                                            Logger::error("Failed to wait for events: {}", std::strerror(errno));
                                            continue;
                                        }
                                        
                                        for (int i = 0; i < ready; ++i)
                                        {

                                            int fd = events[i].data.fd;

                                            auto it = m_handler.find(fd);

                                            if (it == m_handler.end())
                                            {
                                                Logger::error("No handler found for fd {}", fd);
                                                continue;
                                            }

                                            it->second->handleEvents(fd, events[i].events);
                                        }
                                    } });
}
