#include "logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>

std::shared_ptr<spdlog::logger> Logger::s_logger;

void Logger::init() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("firewall_manager.log", true);
    file_sink->set_level(spdlog::level::trace);

    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    s_logger = std::make_shared<spdlog::logger>("FirewallManager", sinks.begin(), sinks.end());
    s_logger->set_level(spdlog::level::trace);
    spdlog::register_logger(s_logger);
}

std::shared_ptr<spdlog::logger> Logger::get() {
    if (!s_logger) {
        init();
    }
    return s_logger;
}
