#include "MulticastManager.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "log.h"
MulticastManager::MulticastManager(Reactor &react, IApplication *app)
    : m_react(react), m_app(app)
{
    if(m_app)
    {
        m_app->setMulticastManager(this);
    }
}

MulticastManager::~MulticastManager()
{
    if (m_sock != -1)
    {
        m_react.unregisteredFd(m_sock);
        close(m_sock);
    }
}

bool MulticastManager::joinGroup(const std::string &ip, uint16_t port)
{

    m_sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (m_sock == -1)
    {
        Logger::error("Failed to create socket");
        return false;
    }

    int reuse = 1;

    if (setsockopt(
            m_sock,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse,
            sizeof(reuse)) < 0)
    {
        Logger::error("Failed to set SO_REUSEADDR");
        return false;
    }

    int loop = 1;
    if (setsockopt(
            m_sock,
            IPPROTO_IP,
            IP_MULTICAST_LOOP,
            &loop,
            sizeof(loop)) < 0)
    {
        Logger::error("Failed to enable multicast loopback");
        return false;
    }

    int ttl = 5;
    if (setsockopt(
            m_sock,
            IPPROTO_IP,
            IP_MULTICAST_TTL,
            &ttl,
            sizeof(ttl)) < 0)
    {
        Logger::error("Failed to set multicast TTL");
        return false;
    }

    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(
            m_sock,
            (sockaddr *)&addr,
            sizeof(addr)) < 0)
    {
        Logger::error("Failed to bind socket");
        return false;
    }

    ip_mreq mreq{};

    mreq.imr_multiaddr.s_addr = inet_addr(ip.c_str());
    mreq.imr_interface.s_addr = INADDR_ANY;

    if (setsockopt(
            m_sock,
            IPPROTO_IP,
            IP_ADD_MEMBERSHIP,
            &mreq,
            sizeof(mreq)) < 0)
    {
        Logger::error("Failed to add multicast membership");
        return false;
    }

    m_groupIp = ip;
    m_port = port;

    m_react.registerFd(
        m_sock,
        EPOLLIN,
        this);
    
    Logger::info("Joined multicast group {}:{} successfully ✅", ip, port);

    return true;
}

bool MulticastManager::sendtoGroup(const std::string &msg)
{
    if (m_sock == -1 || m_groupIp.empty() || m_port == 0)
    {
        Logger::error("Multicast group is not initialized");
        return false;
    }

    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr =
        inet_addr(m_groupIp.c_str());

    ssize_t sent = sendto(
        m_sock,
        msg.c_str(),
        msg.size(),
        0,
        (sockaddr *)&addr,
        sizeof(addr));

    if (sent < 0)
    {
        Logger::error("Failed to send multicast message");
        return false;
    }

    Logger::info("Sent message to multicast group {}:{} successfully ✅", m_groupIp, m_port);

    return true;
}

void MulticastManager::handleEvents(int fd, uint32_t events)
{

    if (!(events & EPOLLIN))
        return;

    char buffer[2048];

    sockaddr_in sender{};
    socklen_t len = sizeof(sender);

    ssize_t bytes =
        recvfrom(
            m_sock,
            buffer,
            sizeof(buffer),
            0,
            (sockaddr *)&sender,
            &len);

    if (bytes > 0)
    {
        std::string msg(buffer, bytes);

        if (m_app)        {
            m_app->onMultiCastData(m_groupIp.c_str(), msg.c_str());
        }
    }
}