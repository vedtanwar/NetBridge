#ifndef IAPPLICATION_H
#define IAPPLICATION_H

class MulticastManager;
class ClientSession;

class IApplication
{
private:
    MulticastManager* m_mcast{nullptr};

public:
    IApplication() {};
    virtual ~IApplication() = default;

    virtual void onTimerExpired(int timerId) = 0;
    virtual void onClientConnected(int clientFd) = 0;
    virtual void onClientDisconnected(int clientFd) = 0;
    virtual void onMessageReceived(ClientSession* session, const char* data) = 0;
    virtual void onMultiCastData(const char* group, const char* data) = 0;
    virtual void sendToClient(const std::string& data) = 0;

    void setMulticastManager(MulticastManager* mcast)
    {
        m_mcast = mcast;
    }

    MulticastManager* getMulticastManager()
    {
        return m_mcast;
    }
};

#endif // IAPPLICATION_H
