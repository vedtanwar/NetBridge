#include "TimerManager.h"
#include "log.h"
#include "Reactor.h"
#include "IApplication.h"
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>

TimerManager::TimerManager(Reactor &reactor, IApplication *app)
    : m_reactor(reactor), m_app(app)
{
    if (!Initialize())
    {
        Logger::error("Failed to initialize Timer Manager❌");
    }
}

TimerManager::~TimerManager()
{
    for (int i = 0; i < DG_MAX_TIMER; i++)
    {
        if (m_timers[i].timerFd != -1)
        {
            close(m_timers[i].timerFd);
        }
    }
}

bool TimerManager::Initialize()
{

    for (int i = 0; i < DG_MAX_TIMER; i++)
    {
        int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

        if (tfd == -1)
        {
            Logger::error("Failed to Create Timer fd: {}", std::strerror(errno));
            return false;
        }

        m_timers[i].timerFd = tfd;

        m_reactor.registerFd(
            tfd,
            EPOLLIN,
            this);
    }

    Logger::info("Timer Manager Initialized successfully ✅ with {} timers", DG_MAX_TIMER);

    return true;
}

bool TimerManager::startTimer(TimerId timerId, u_int32_t interval, bool periodic)
{

    int index = static_cast<int>(timerId);

    itimerspec ts{};

    ts.it_value.tv_sec = interval;

    if (periodic)
    {
        ts.it_interval.tv_sec = interval;
    }

    if (timerfd_settime(m_timers[index].timerFd, 0, &ts, nullptr) == -1)
    {
        Logger::error("Failed to start the Timer for TimerId : {}", index);
        return false;
    }

    m_timers[index].active = true;
    m_timers[index].periodic = periodic;

    Logger::info("Timer {} started successfully with interval {}", index, interval);
    return true;
}

void TimerManager::stopTimer(TimerId timerId)
{
    int index = static_cast<int>(timerId);

    itimerspec ts{};

    timerfd_settime(m_timers[index].timerFd, 0, &ts, nullptr);

    m_timers[index].active = false;
}

void TimerManager::handleEvents(int fd, uint32_t events)
{

    if (!(events & EPOLLIN))
    {
        return;
    }

    for (int i = 0; i < DG_MAX_TIMER; i++)
    {
        if (fd != m_timers[i].timerFd)
        {
            continue;
        }
        u_int64_t expirations;

        read(fd, &expirations, sizeof(expirations));

        if (m_app)
        {

            m_app->onTimerExpired(i);
        }
        break;
    }
}
