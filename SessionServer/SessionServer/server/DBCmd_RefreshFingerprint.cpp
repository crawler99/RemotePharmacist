#include "DBCmd_RefreshFingerprint.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include "Enums.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_RefreshFingerprint::DBCmd_RefreshFingerprint(Poco::UInt32 userId)
        : _userId(userId)
    {
        reset();
    }

    bool DBCmd_RefreshFingerprint::execute(PGconn *pConn)
    {
        std::stringstream ss;
        ss << "INSERT INTO tbl_Pharmacists_Activity_Log VALUES(" << _userId << ", NOW())";

        _pResult = PQexec(pConn, ss.str().c_str());

        bool succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        PQclear(_pResult);
        return succ;
    }

    void DBCmd_RefreshFingerprint::reset()
    {        
    }

} }