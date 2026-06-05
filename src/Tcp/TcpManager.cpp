#include "TcpManager.h"
#include "log.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

TcpManager::TcpManager(IApplication *app, Reactor& react)
    : m_app(app), m_react(react)
{
}

TcpManager::~TcpManager()
{

    if (m_listenFd != -1)
    {
        close(m_listenFd);
        m_listenFd = -1;
    }
}

bool TcpManager::startServer(uint16_t port)
{

    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);

    if (m_listenFd == -1)
    {
        Logger::error("Socket Creation Failed!!: {}", std::strerror(errno));
        return false;
    }

    if (!SetNonBlocking(m_listenFd))
    {
        Logger::error("Set Non-blocking failed!! : {}", std::strerror(errno));
        return false;
    }

    int opt = 1;
    if (setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        Logger::error("Set Socket Option Failed!!: {}", std::strerror(errno));
        close(m_listenFd);
        m_listenFd = -1;
        return false;
    }

    if (setsockopt(m_listenFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        Logger::error("Set Socket Option Failed!!: {}", std::strerror(errno));
        close(m_listenFd);
        m_listenFd = -1;
        return false;
    }

    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(m_listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        Logger::error("Socket Bind Failed !! : {}", std::strerror(errno));
        close(m_listenFd);
        m_listenFd = -1;
        return false;
    }

    if(listen(m_listenFd, SOMAXCONN))
    {
        Logger::error("Listening Clients failed !! {}", std::strerror(errno));
        close(m_listenFd);
        m_listenFd = -1;
        return false;
    }


    m_react.registerFd(m_listenFd, EPOLLIN, this);

    Logger::info("Socket Creation Success ✅ \nTcp Server is listening at port : {}", port);

    return true;
}

void TcpManager::handleEvents(int fd, uint32_t events)
{

    if(fd == m_listenFd)
    {
        acceptClient();
    }
    
}

void TcpManager::acceptClient()
{

    while(true)
    {
        int clientFd = accept(m_listenFd, nullptr, nullptr);

        if(clientFd == -1)
        {
            if(EAGAIN == errno || EWOULDBLOCK == errno)
            {
                break;
            }
            return;
        }

        SetNonBlocking(clientFd);

        auto session = std::make_unique<ClientSession>(clientFd, *this, m_app);

        m_react.registerFd(clientFd, EPOLLIN, session.get());

        m_clients[clientFd] = std::move(session);

        if(m_app)
        {
            m_app->onClientConnected(clientFd);
        }
    }
    
}

void TcpManager::removeClient(int clientFd)
{
    m_react.unregisteredFd(clientFd);

    if(m_app)
    {
        m_app->onClientDisconnected(clientFd);
    }

    m_clients.erase(clientFd);
}

bool TcpManager::SetNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1)
    {
        return false;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        return false;
    }
    return true;
}


