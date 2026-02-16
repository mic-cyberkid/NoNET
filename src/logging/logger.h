#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

class Logger {
public:
    static void init();
    static std::shared_ptr<spdlog::logger> get();

    template<typename... Args>
    static void info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
        get()->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
        get()->error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
        get()->warn(fmt, std::forward<Args>(args)...);
    }

private:
    static std::shared_ptr<spdlog::logger> s_logger;
};
