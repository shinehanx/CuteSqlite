#include "stdafx.h"
#include "Log.h"
#include "ResourceUtil.h"

QLog::Logger& GetInstance() {
    static QLog::Logger m_instance;
    return m_instance;
}

namespace QLog {
    Logger::Logger() {
        if (::_access("logs", 0) == -1) {
            ::_mkdir("logs");
        }

        //设置为异步日志
        //spdlog::set_async_mode(32768);  // 必须为 2 的幂
        std::vector<spdlog::sink_ptr> sinkList;

#if 1
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::debug);
        consoleSink->set_pattern("%Y-%m-%d %H:%M:%S.%e.%f [%l] [%t] - <%s,line:%#>|<%!> - %v");
        sinkList.push_back(consoleSink);
#endif
		std::wstring logfile = ResourceUtil::getProductBinDir() + L"logs/application.log";
        auto dailySink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(logfile.c_str(), 2, 30);
        dailySink->set_level(spdlog::level::debug);
		dailySink->set_pattern("%Y-%m-%d %H:%M:%S.%e.%f [%l] [%t] - <%s,line:%#>|<%!> - %v");
        sinkList.push_back(dailySink);

        nml_logger = std::make_shared<spdlog::logger>("both", begin(sinkList), end(sinkList));
        //register it if you need to access it globally
        spdlog::register_logger(nml_logger);

        // 设置日志记录级别
#ifdef _DEBUG
        nml_logger->set_level(spdlog::level::trace);
#else
        nml_logger->set_level(spdlog::level::info);
#endif

        //设置当出发 err 或更严重的错误时立刻刷新日志到  disk .
        nml_logger->flush_on(spdlog::level::warn);

		//%e - Millisecond part of the current second 0-999
		//%f - Microsecond part of the current second 0-999999
		//%F - Nanosecond part of the current second 0-999999999
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e.%f [%l] [%t] - <%s,line:%#>|<%!> - %v");
        spdlog::flush_every(std::chrono::seconds(2));

    }

    Logger::~Logger() {
        spdlog::drop_all();
    }
}
