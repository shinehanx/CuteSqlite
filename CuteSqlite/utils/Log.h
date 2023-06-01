#ifndef _LOG_0B0512CC_B1CC_4483_AF05_1914E7F7D4DA_
#define _LOG_0B0512CC_B1CC_4483_AF05_1914E7F7D4DA_

#include <string>
#include <corecrt_io.h>

#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace QLog {
    class Logger {
      public:
        auto getLogger() {
            return nml_logger;
        }

        Logger();
        ~Logger();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

      private:
        std::shared_ptr<spdlog::logger> nml_logger;
    };
}
QLog::Logger& GetInstance();

#define SPDLOG_LOGGER_CALL_(level, ...) GetInstance().getLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)
#define Q_TRACE(...)  SPDLOG_LOGGER_CALL_(spdlog::level::trace,__VA_ARGS__)
#define Q_DEBUG(...)  SPDLOG_LOGGER_CALL_(spdlog::level::debug,__VA_ARGS__)
#define Q_INFO(...)   SPDLOG_LOGGER_CALL_(spdlog::level::info,__VA_ARGS__)
#define Q_LOG(...)   SPDLOG_LOGGER_CALL_(spdlog::level::info,__VA_ARGS__)
#define Q_WARN(...)   SPDLOG_LOGGER_CALL_(spdlog::level::warn,__VA_ARGS__)
#define Q_ERROR(...)  SPDLOG_LOGGER_CALL_(spdlog::level::err,__VA_ARGS__)
#define LogCritical(...) SPDLOG_LOGGER_CALL_(spdlog::level::critical,__VA_ARGS__)
#define LogCriticalIf(b, ...)               \
    do {                                  		    \
			if ((b)) {               			  \
				 SPDLOG_LOGGER_CALL_(spdlog::level::critical,__VA_ARGS__); \
			}                                      \
	} while (0)

#ifdef WIN32
#define errcode WSAGetLastError()
#endif

#endif//_LOG_0B0512CC_B1CC_4483_AF05_1914E7F7D4DA_
