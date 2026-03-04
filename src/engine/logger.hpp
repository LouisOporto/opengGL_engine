#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <filesystem>

#define LOG_COLOR_RESET "\x1b[0m"
#define LOG_COLOR_LOG "\x1b[34m"
#define LOG_COLOR_WARN "\x1b[30;43m"
#define LOG_COLOR_ERROR "\x1b[41m"

class Logger {
 private:
    inline static char currentSessionTime[1024];
    inline static bool usingSession = false;
 public:
    static bool init() {
        if (!std::filesystem::is_directory("./logs")) {
            if (!std::filesystem::create_directory("./logs")) {
                return false;
            }
        }
        std::time_t t = std::time(nullptr);
        std::tm now;
        localtime_s(&now, &t);

        snprintf(currentSessionTime, sizeof(currentSessionTime), "./logs/%02d-%02d %02d.%02d.%02d-session.txt", now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
        return usingSession = true;
    }

    template <typename... Args>
    static void Log(const char* msg, Args... args) {
        std::time_t t = std::time(nullptr);
        std::tm now;
        localtime_s(&now, &t);

        char message[1024];
        char timezone[1024];
        snprintf(message, sizeof(message), msg, args...);
        snprintf(timezone, sizeof(timezone), "[%02d:%02d:%02d]", now.tm_hour, now.tm_min, now.tm_sec);

        printf(LOG_COLOR_LOG "%s LOG: ", timezone);
        printf(msg, args...);
        printf(LOG_COLOR_RESET "\n");

        std::fstream file("./logs/log.txt", std::ios::out | std::ios::app);
        file << timezone << " LOG: " << message << std::endl;
        file.close();

        if (usingSession) {
            std::fstream session(currentSessionTime, std::ios::out | std::ios::app);
            session << timezone << " LOG: " << message << std::endl;
            session.close();
        }
    }

    template <typename... Args>
    static void Warn(const char* msg, Args... args) {
        std::time_t t = std::time(nullptr);
        std::tm now;
        localtime_s(&now, &t);

        char message[1024];
        char timezone[1024];
        snprintf(message, sizeof(message), msg, args...);
        snprintf(timezone, sizeof(timezone), "[%02d:%02d:%02d]", now.tm_hour, now.tm_min, now.tm_sec);
        
        printf(LOG_COLOR_WARN "%s WARN: ", timezone);
        printf(msg, args...);
        printf(LOG_COLOR_RESET "\n");

        std::fstream file("log.txt", std::ios::out | std::ios::app);
        file << timezone <<" WARN: " << message << std::endl;
        file.close();

        if (usingSession) {
            std::fstream session(currentSessionTime, std::ios::out | std::ios::app);
            session << timezone <<" WARN: " << message << std::endl;
            session.close();
        }
    }

    template <typename... Args>
    static void Error(const char* msg, Args... args) {
        std::time_t t = std::time(nullptr);
        std::tm now;
        localtime_s(&now, &t);

        char message[1024];
        char timezone[1024];
        snprintf(message, sizeof(message), msg, args...);
        snprintf(timezone, sizeof(timezone), "[%02d:%02d:%02d]", now.tm_hour, now.tm_min, now.tm_sec);
        
        printf(LOG_COLOR_ERROR "%s ERROR: ", timezone);
        printf(msg, args...);
        printf(LOG_COLOR_RESET "\n");

        std::fstream file("log.txt", std::ios::out | std::ios::app);
        file << timezone << " ERROR: " << message << std::endl;
        file.close();

        if (usingSession) {
            std::fstream session(currentSessionTime, std::ios::out | std::ios::app);
            session << timezone <<" ERROR: " << message << std::endl;
            session.close();
        }
    }
};

#endif  // _LOGGER_H_