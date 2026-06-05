#ifndef MULTICASTMANAGER_H
#define MULTICASTMANAGER_H

#include "IEventHandler.h"
#include "IApplication.h"
#include "Reactor.h"
#include<iostream>
#include<unistd.h>

class IApplication;

class MulticastManager : public IEventHandler
{
private:
    /* data */
    Reactor& m_react;
    IApplication* m_app{nullptr};
    int m_sock{-1};

    std::string m_groupIp;
    uint16_t m_port{0};

    
public:
    MulticastManager(Reactor& react, IApplication* app);
    ~MulticastManager();

    bool joinGroup(const std::string& ip, uint16_t port);
    bool sendtoGroup(const std::string& msg);
    virtual void handleEvents(int fd, uint32_t events) override;

};

#endif // MULTICASTMANAGER_H
