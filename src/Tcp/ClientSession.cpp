#include "ClientSession.h"
#include "log.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>

ClientSession::ClientSession(int fd, TcpManager &tcp, IApplication *app)
    : m_Fd{fd}, m_tcp{tcp}, m_app{app}
{
}

ClientSession::~ClientSession()
{
    if (m_Fd != -1)
    {
        close(m_Fd);
        m_Fd = -1;
    }
}

int ClientSession::getFd() const
{
    return m_Fd;
}

bool ClientSession::receiveData()
{
    uint8_t buffer[4096];

    while (true)
    {
        ssize_t bytes = recv(m_Fd, buffer, sizeof(buffer), 0);

        if (bytes > 0)
        {
            m_rxbuffer.insert(m_rxbuffer.end(), buffer, buffer + bytes);
            continue;
        }

        if (bytes == 0)
        {
            return false;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            break;
        }

        return false;
    }

    return true;
}

bool ClientSession::sendPendingData()
{
    while (!m_txbuffer.empty())
    {
        ssize_t bytes = send(m_Fd, m_txbuffer.data(), m_txbuffer.size(), 0);

        if (bytes > 0)
        {
            m_txbuffer.erase(m_txbuffer.begin(),
                             m_txbuffer.begin() + bytes);
            continue;
        }

        if (bytes == 0)
        {
            Logger::error("Nothing to send !! {}", std::strerror(errno));
            return false;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            break;
        }

        return false;
    }

    Logger::info("Server successfully Respinded to Client {} !!", m_Fd);

    return true;
}

void ClientSession::queueData(const uint8_t *data, size_t len)
{

    if (data == nullptr || len == 0)
    {
        Logger::error("Data is Empty to Queue");
        return;
    }

    m_txbuffer.insert(m_txbuffer.end(),
                      data,
                      data + len);
}

const std::vector<uint8_t> &ClientSession::getRxBuffer() const
{
    return m_rxbuffer;
}

void ClientSession::consumeRxData(size_t bytes)
{

    if (bytes >= m_rxbuffer.size())
    {
        m_rxbuffer.clear();
        return;
    }

    m_rxbuffer.erase(m_rxbuffer.begin(), m_rxbuffer.begin() + bytes);
}

bool ClientSession::hasPendingTx() const
{
    return !m_txbuffer.empty();
}

void ClientSession::handleEvents(int fd, uint32_t events)
{

    if (events & (EPOLLERR | EPOLLHUP))
    {
        disconnect();
        return;
    }

    if (events & EPOLLIN)
    {
        if (!receiveData())
        {
            disconnect();
            Logger::info("Client Disconnected: {}", fd);
            return;
        }
    }

    if (m_app && !m_rxbuffer.empty())
    {
        std::string data(m_rxbuffer.begin(), m_rxbuffer.end());
        m_app->onMessageReceived(this, data.c_str());
    }

    if (!sendPendingData())
    {
        disconnect();
        Logger::info("Client Disconnected while sending data: {}", fd);
        return;
    }
}

void ClientSession::disconnect()
{
    m_tcp.removeClient(m_Fd);
}


void ClientSession::sendTxMessage(const char *data)
{
    this->m_txbuffer.insert(this->m_txbuffer.end(), data, data + strlen(data));
    this->sendPendingData();
}