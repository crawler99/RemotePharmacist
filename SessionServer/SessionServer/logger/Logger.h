#pragma once

#include "Poco/Logger.h"
#include "Poco/Util/XMLConfiguration.h"
#include "Poco/LocalDateTime.h"

namespace sserver {

    class Logger
    {
    public:
        static bool init(char *appName, Poco::Util::XMLConfiguration *pConfig);
        static Poco::Logger& instance();
        static Poco::LocalDateTime getRefDateTime();

    private:
        Logger();

        // save the starting up time for global reference
        static Poco::LocalDateTime _dateTime;
    };

    #define LOG_FATAL(m)       sserver::Logger::instance().fatal(m,__FILE__,__LINE__);
    #define LOG_CRITICAL(m)    sserver::Logger::instance().critical(m,__FILE__,__LINE__);
    #define LOG_ERROR(m)       sserver::Logger::instance().error(m,__FILE__,__LINE__);
    #define LOG_WARNING(m)     sserver::Logger::instance().warning(m,__FILE__,__LINE__);
    #define LOG_NOTICE(m)      sserver::Logger::instance().notice(m,__FILE__,__LINE__);
    #define LOG_INFO(m)        sserver::Logger::instance().information(m,__FILE__,__LINE__);
    #define LOG_DEBUG(m)       sserver::Logger::instance().debug(m,__FILE__,__LINE__);
    #define LOG_TRACE(m)       sserver::Logger::instance().trace(m,__FILE__,__LINE__);
}
