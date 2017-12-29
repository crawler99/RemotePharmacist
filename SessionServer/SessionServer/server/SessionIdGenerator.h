#pragma once

#include "../logger/Logger.h"
#include "Poco/Mutex.h"
#include "tbb/atomic.h"
#include <map>

namespace sserver { namespace server { 

    class SessionIdGenerator
    {
    public:
        // check if a given session id belongs to the specified user role
        static inline bool checkRole(Poco::UInt32 sessionId, Poco::UInt8 userRole)
        {
            return (0 == ((userRole << 24) ^ (sessionId & 0xFF000000)));
        }
        // generate a new session id based on user role
        static Poco::UInt32 getNextSessionId(Poco::UInt8 userRole);

    private:
        static std::map<Poco::UInt8, Poco::UInt32> _seeds;
        static Poco::FastMutex _mtx;
    };

} }