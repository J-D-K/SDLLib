#pragma once
#include <fstream>
#include <string_view>

class Logger final
{
    public:
        // No copying or moving.
        Logger(const Logger &)            = delete;
        Logger(Logger &&)                 = delete;
        Logger &operator=(const Logger &) = delete;
        Logger &operator=(Logger &&)      = delete;

        static void log_line(std::string_view line)
        {
            Logger &logger        = Logger::instance();
            std::ofstream &stream = logger.m_debugFile;

            stream << line << std::endl;
            stream.flush();
        }

    private:
        /// @brief Actual file we're writing to.
        std::ofstream m_debugFile{"sdmc:/debug.txt"};

        /// @brief Private constructor.
        Logger() = default;

        /// @brief Instance getter.
        static Logger &instance()
        {
            static Logger logger;
            return logger;
        }
};