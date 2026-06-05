#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include "IEventHandler.h"
#include "IApplication.h"
#include "ClientSession.h"
#include "Reactor.h"

#include <memory>
#include <unordered_map>

class ClientSession;

class TcpManager : public IEventHandler
{
private:
    /* data */

    IApplication* m_app;
    Reactor& m_react;
    int m_listenFd;

    std::unordered_map<int, std::unique_ptr<ClientSession>> m_clients;

    
public:
    TcpManager(IApplication* app, Reactor& react);
    ~TcpManager();

    bool startServer(uint16_t port);
    virtual void handleEvents(int fd, uint32_t events) override;
    void removeClient(int clientFd);

    void acceptClient();

    bool SetNonBlocking(int fd);
};

#endif // TCPMANAGER_H
