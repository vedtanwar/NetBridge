#include "TcpClient.h"
#include "log.h"

int main()
{
    Logger::init();
    TcpClient client;

    if (!client.ConnectToServer(
            "127.0.0.1",
            13400))
    {
        return -1;
    }

    client.run();

    return 0;
}