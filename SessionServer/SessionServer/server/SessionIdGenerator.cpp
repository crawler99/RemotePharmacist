#include "SessionIdGenerator.h"

namespace sserver { namespace server { 

    Poco::FastMutex SessionIdGenerator::_mtx;
    std::map<Poco::UInt8, Poco::UInt32> SessionIdGenerator::_seeds;

    Poco::UInt32 SessionIdGenerator::getNextSessionId(Poco::UInt8 userRole)
    {
        Poco::FastMutex::ScopedLock lock(_mtx);

        Poco::UInt32 incPart = 0;

        std::map<Poco::UInt8, Poco::UInt32>::iterator iter = _seeds.find(userRole);
        if (_seeds.end() == iter)
        {
            incPart = 1;
            _seeds[userRole] = incPart;
        }
        else
        {
            incPart = iter->second;
            if (++incPart > 0xFFFFFF)
            {
                incPart = 1;
            }
            iter->second = incPart;
        }
        
        return ((userRole << 24) ^ incPart);
    }

} }