#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>

#define LOG_COLOR_RESET "\x1b[0m"
#define LOG_COLOR_LOG "\x1b[34m"
#define LOG_COLOR_WARN "\x1b[30;43m"
#define LOG_COLOR_ERROR "\x1b[41m"

class Logger {
   public:
    template <typename... Args>
    static void Log(const char* msg, Args... args) {
        std::time_t t = std::time(nullptr);
        std::tm now;
        localtime_s(&now, &t);
        printf(LOG_COLOR_LOG "[%d:%d:%d] LOG: ", now.tm_hour, now.tm_min,
               now.tm_sec);
        printf(msg, args...);
        printf(LOG_COLOR_RESET "\n");

        char message[1024];
        snprintf(message, sizeof(message), msg, args...);
        std::fstream file("log.txt", std::ios::out | std::ios::app);
        file << "[" << std::setfill('0') << std::setw(2) << now.tm_hour << ":"
             << std::setfill('0') << std::setw(2) << now.tm_min << ":"
             << std::setfill('0') << std::setw(2) << now.tm_sec
             << "] LOG: " << message << std::endl;
        file.close();
    }

    template <typename... Args>
    static void Warn(const char* msg, Args... args) {
        std::time_t t = std::time(nullptr);
        std::tm now;
        localtime_s(&now, &t);
        printf(LOG_COLOR_WARN "[%d:%d:%d] WARN: ", now.tm_hour, now.tm_min,
               now.tm_sec);
        printf(msg, args...);
        printf(LOG_COLOR_RESET "\n");

        char message[1024];
        snprintf(message, sizeof(message), msg, args...);
        std::fstream file("log.txt", std::ios::out | std::ios::app);
        file << "[" << std::setfill('0') << std::setw(2) << now.tm_hour << ":"
             << std::setfill('0') << std::setw(2) << now.tm_min << ":"
             << std::setfill('0') << std::setw(2) << now.tm_sec
             << "] WARN: " << message << std::endl;
        file.close();
    }

    template <typename... Args>
    static void Error(const char* msg, Args... args) {
        std::time_t t = std::time(nullptr);
        std::tm now;
        localtime_s(&now, &t);
        printf(LOG_COLOR_ERROR "[%d:%d:%d] ERROR: ", now.tm_hour, now.tm_min,
               now.tm_sec);
        printf(msg, args...);
        printf(LOG_COLOR_RESET "\n");

        char message[1024];
        snprintf(message, sizeof(message), msg, args...);
        std::fstream file("log.txt", std::ios::out | std::ios::app);
        file << "[" << std::setfill('0') << std::setw(2) << now.tm_hour << ":"
             << std::setfill('0') << std::setw(2) << now.tm_min << ":"
             << std::setfill('0') << std::setw(2) << now.tm_sec
             << "] ERROR: " << message << std::endl;
        file.close();
    }
};

#endif  // _LOGGER_H_