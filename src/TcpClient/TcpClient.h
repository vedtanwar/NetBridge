#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include<string>

class TcpClient
{
private:
    /* data */

    int m_socketFd;

public:
    TcpClient();
    ~TcpClient();

    bool ConnectToServer(const std::string &ip, int port);

    bool sendMessage(std::string messgae);

    bool receiveMessage();
    
    void run();

};

#endif // TCPCLIENT_H