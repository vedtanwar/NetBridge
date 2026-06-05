#include "log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

std::shared_ptr<spdlog::logger> Logger::m_logger = nullptr;

void Logger::init()
{

    m_logger = spdlog::stdout_color_mt("Networking");

    spdlog::set_pattern(
        "[%H:%M:%S] [%^%l%$] %v");

    spdlog::set_level(spdlog::level::trace);

    m_logger->info("Logger initialized");
}

void Logger::shutdown()
{
    spdlog::shutdown();
}