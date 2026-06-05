#include "Reactor.h"
#include "log.h"
#include "TimerManager.h"
#include "MulticastManager.h"
#include "TcpManager.h"
#include "Application.cpp"

#include <string>
#include <thread>

int main()
{
    Logger::init();
    Logger::info("Starting application...🎬");

    Reactor reactor;
    Application app;

    /*
        Timer Manager
    */
    TimerManager timerManager(reactor, &app);

    timerManager.startTimer(TimerId::Timer1, 5);

    /*
        TCP Manager
    */

    TcpManager tcpManager(&app, reactor);

    if(!tcpManager.startServer(13400))
    {
        Logger::error("Failed to start TCP Server");
        return -1;
    }

    // -----------------------------
    // Multicast Manager
    // -----------------------------

    MulticastManager mcast(
        reactor, &app);

    if (!mcast.joinGroup(
            "239.1.1.1",
            13400))
    {
        std::cout << "Multicast failed\n";
        return -1;
    }
    

    // optional test sender
    std::thread sender([&mcast]() {
        std::string line;

        std::cout << "Type multicast messages and press Enter:\n";

        while (std::getline(std::cin, line))
        {
            if (line.empty())
                continue;

            if (!mcast.sendtoGroup(line))
            {
                Logger::error("Failed to publish multicast message");
            }
        }
    });
    sender.detach();

    // RUN EVENT LOOP
    // -----------------------------
    Logger::info("Event loop running...🚀");

    reactor.run();


    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Logger::shutdown();

}