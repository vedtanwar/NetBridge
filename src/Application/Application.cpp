#include "IApplication.h"
#include "ClientSession.h"
#include "TimerType.h"
#include "MulticastManager.h"
#include "log.h"
#include <iostream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <functional>

struct Request
{
    ClientSession *session;
    std::string message;
};

class Application : public IApplication
{
private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stopProcessing{false};

    std::queue<Request> m_requestQueue;
    ClientSession *m_currentSession{nullptr};
    std::thread m_processingThread;

public:
    Application() : m_processingThread(&Application::processRequestsLoop, this)
    {
    }

    ~Application() override
    {
        m_stopProcessing.store(true);
        m_condition.notify_all();
        if (m_processingThread.joinable())
        {
            m_processingThread.join();
        }
    }

    void processRequestsLoop()
    {
        while (!m_stopProcessing.load())
        {
            Request req;

            {
                std::unique_lock<std::mutex> lock(m_mutex);

                // Wait until there's a request in the queue
                m_condition.wait(lock, [this]
                                 { return !m_requestQueue.empty() || m_stopProcessing.load(); });

                if (m_requestQueue.empty())
                {
                    continue;
                }

                // Get the request
                req = m_requestQueue.front();
                m_requestQueue.pop();
                m_currentSession = req.session;
            }
            processRequest(req);
        }
    }

    void processRequest(const Request &req)
    {
        Logger::info("Processing request from Client {}: {}", req.session->getFd(), req.message);
        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // Echo the message back to the client
        sendToClient("Echo: " + req.message);
    }

    virtual void onTimerExpired(int timerId) override
    {
        Logger::info("Timer Expired for id : {}", timerId);
    }

    virtual void onClientConnected(int clientFd) override
    {
        Logger::info("New Client Connected to Server : {}", clientFd);
    }

    virtual void onClientDisconnected(int clientFd) override
    {
        Logger::info("Client DisConnected from Server : {}", clientFd);
    }

    virtual void onMessageReceived(ClientSession *session, const char *data) override
    {
        // Add request to queue
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_requestQueue.push({session, data ? std::string(data) : std::string()});
        }

        // Notify the processing thread
        m_condition.notify_one();

        Logger::info("Request queued from Client {}", session->getFd());
    }

    virtual void onMultiCastData(const char *group, const char *data) override
    {
        Logger::info("Multi Cast Data received from {}: {}", group, data);
    }

    virtual void sendToClient(const std::string &data)
    {
        if (m_currentSession)
        {
            m_currentSession->sendTxMessage(data.c_str());
        }
    }
};