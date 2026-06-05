#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <unistd.h>
#include "IEventHandler.h"
#include "TcpManager.h"
#include "IApplication.h"

class TcpManager;

enum class ConnectionState
{
    CONNECTED,
    DISCONNECTED,
    AUTHENTICATED
};

class ClientSession : public IEventHandler
{
private:
    /* data */

    int m_Fd {-1};
    IApplication *m_app{nullptr};
    TcpManager& m_tcp;
    

    std::vector<uint8_t> m_rxbuffer;
    std::vector<uint8_t> m_txbuffer;

    ConnectionState m_state{ConnectionState::CONNECTED};

    
public:
    explicit ClientSession(int fd, TcpManager& tcp, IApplication *app);
    ~ClientSession();

    int getFd() const;
    bool receiveData();
    bool sendPendingData();
    void queueData(const uint8_t* data, size_t len);
    const std::vector<uint8_t>& getRxBuffer() const;
    void consumeRxData(size_t bytes);
    bool hasPendingTx() const;
    void sendTxMessage(const char* data);

    void disconnect();

    virtual void handleEvents(int fd, uint32_t events) override;
};

#endif // CLIENTSESSION_H
