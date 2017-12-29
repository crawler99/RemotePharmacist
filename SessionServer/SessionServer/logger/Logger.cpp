#include "Logger.h"
#include "../util/Utils.h"
#include "Poco/AutoPtr.h" 
#include "Poco/FileChannel.h"  
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Net/DNS.h"
#include "Poco/Process.h"
#include <cassert>
#include <iostream>

// ------------------------------------------------
#ifdef WIN32
    #include <process.h>
    #ifndef getpid
        #define getpid _getpid
    #endif
#else
    #include <unistd.h>
#endif
// ------------------------------------------------

namespace sserver {

    Poco::LocalDateTime Logger::_dateTime;

    Logger::Logger()
    {
    }

    Poco::Logger& Logger::instance()
    {
        return Poco::Logger::root();
    }

    Poco::LocalDateTime Logger::getRefDateTime()
    {
        return _dateTime;
    }

    bool Logger::init(char *appName, Poco::Util::XMLConfiguration *pConfig)
    {
        if (NULL == pConfig) return false;
        _dateTime = Poco::LocalDateTime();
        
        try
        {
            // set level
            int level = pConfig->getInt("Logger.LogLevel");
            Poco::Logger::root().setLevel(level);

            // create log dir
            std::string dir = pConfig->getString("Logger.LogFileDir");
            dir += "/" + Poco::DateTimeFormatter::format(_dateTime, "%Y-%m-%d");
            Poco::File logDir(dir);
            logDir.createDirectories();
            
            // determine log file name
            Poco::Path appPath(appName);
            std::string logPath = dir + "/" + appPath.getBaseName();
            logPath += Poco::DateTimeFormatter::format(_dateTime, "_%Y-%m-%d_%H_%M_%S_");
            logPath += Poco::Net::DNS::hostName();

            std::string pidStr;
            Utils::num2String<int>(getpid(), pidStr);
            if (!pidStr.empty())
            {
                logPath += "." + pidStr;
            }
            logPath += ".log";
            
            // set log channel
            Poco::AutoPtr<Poco::Channel> fileChannel(new Poco::FileChannel());  
            fileChannel->setProperty("path", logPath);  
            fileChannel->setProperty("rotation", "daily");
            fileChannel->setProperty("times", "local");
            fileChannel->setProperty("archive", "timestamp");  
            fileChannel->setProperty("compress", "true");

            Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());  
            patternFormatter->setProperty("pattern", "%Y%m%d %H:%M:%S.%F %I %q - %U:%u - %t");
			patternFormatter->setProperty("times", "local");

            Poco::AutoPtr<Poco::Channel> channel = new Poco::FormattingChannel(patternFormatter, fileChannel);
            Poco::Logger::root().setChannel(channel.get());
        }
        catch (Poco::NotFoundException &e)
        {
            std::cout << "Missing configuration item: " << e.message() << std::endl;
            return false;
        }
        catch (...)
        {
            std::cout << "Error when initializing logger" << std::endl;
            return false;
        }

        return true;
    }

}