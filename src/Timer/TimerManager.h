#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H
#include "IEventHandler.h"
#include "TimerType.h"

#include <array>
#include<iostream>

class Reactor;
class IApplication;

struct TimerInfo
{
    /* data */
    int timerFd {-1};
    bool active {false};
    bool periodic {false};
};

class TimerManager : public IEventHandler
{
private:
    /* data */

    Reactor& m_reactor;
    IApplication* m_app;

    std::array<TimerInfo, DG_MAX_TIMER> m_timers;

    
public:
    TimerManager(Reactor& reactor, IApplication* app);
    ~TimerManager();

    bool Initialize();
    bool startTimer(TimerId timerId, u_int32_t interval, bool periodic = false);
    void stopTimer(TimerId timerId);

    virtual void handleEvents(int fd, uint32_t events) override;

};





#endif // TIMERMANAGER_H
