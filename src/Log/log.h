#ifndef LOG_H
#define LOG_H

#include <memory>
#include <string>

#include <spdlog/fmt/fmt.h>
#include <spdlog/logger.h>

class Logger
{
public:
    static void init();

    static void shutdown();

    template <typename... Args>
    static void info(fmt::format_string<Args...> fmt,
                     Args &&...args)
    {

        if (m_logger)
        {
            m_logger->info(fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    static void warn(fmt::format_string<Args...> fmt,
                     Args &&...args)
    {

        if (m_logger)
        {
            m_logger->warn(fmt, std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    static void error(fmt::format_string<Args...> fmt,
                      Args &&...args)
    {

        if (m_logger)
        {
            m_logger->error(fmt, std::forward<Args>(args)...);
        }
    }

private:
    static std::shared_ptr<spdlog::logger> m_logger;
};

#endif // LOG_H
