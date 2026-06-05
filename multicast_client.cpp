#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    int reuse = 1;

    setsockopt(sock,
        SOL_SOCKET,
        SO_REUSEADDR,
        &reuse,
        sizeof(reuse));

    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(13400);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock,
        (sockaddr*)&addr,
        sizeof(addr));

    ip_mreq mreq{};

    mreq.imr_multiaddr.s_addr =
        inet_addr("239.1.1.1");

    mreq.imr_interface.s_addr =
        INADDR_ANY;

    setsockopt(sock,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        &mreq,
        sizeof(mreq));

    char buffer[1024];

    std::cout << "Client listening...\n";

    while (true)
    {
        int n = recv(sock,
            buffer,
            sizeof(buffer),
            0);

        if (n > 0)
        {
            std::cout
                << "Received: "
                << std::string(buffer, n)
                << std::endl;
        }
    }
}