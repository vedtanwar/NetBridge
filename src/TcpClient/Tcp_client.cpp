#include "TcpClient.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "log.h"
#include <iostream>

TcpClient::TcpClient()
{
    m_socketFd = -1;
}

TcpClient::~TcpClient()
{
    if (m_socketFd != -1)
    {
        Logger::info("Closing client {}", m_socketFd);
        close(m_socketFd);
    }
}

bool TcpClient::ConnectToServer(const std::string &ip, int port)
{

    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if (m_socketFd == -1)
    {
        Logger::error("Client Socket Creation Failed!!!!");
        return false;
    }

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        Logger::error("Inet_pton error!!!");
        return false;
    }

    if (connect(m_socketFd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
    {
        Logger::error("Client {} connection Failed!!!", m_socketFd);
        return false;
    }

    Logger::info("Connected to Server :- {}:{}", ip, port);
    return true;
}

bool TcpClient::sendMessage(std::string messgae)
{

    ssize_t bytes = send(m_socketFd, messgae.c_str(), messgae.size(), 0);

    if (bytes <= 0)
    {
        Logger::error("Messgae send Failed!!!!");
        return false;
    }
    Logger::info("Message sent to the server : {}", messgae);
    return true;
}

bool TcpClient::receiveMessage()
{

    char buffer[1024] = {0};

    ssize_t bytes = recv(m_socketFd, buffer, 1024, 0);

    if (bytes <= 0)
    {
        Logger::error("Data Received Failed from the server!!!!");
        return false;
    }

    std::string recvMessage;
    recvMessage.assign(buffer, bytes);

    Logger::info("Data Received from the server : {}", recvMessage);

    return true;
}

void TcpClient::run()
{

    std::string input;

    while (true)
    {
        Logger::info("Enter the Message for the Server : ");

        std::getline(
            std::cin,
            input);

        if ((input == "quit"))
        {
            break;
        }

        if (!sendMessage(input))
        {
            continue;
        }

        // Server now echoes responses - receive the echo back
        if (!receiveMessage())
        {
            continue;
        }
    }
}