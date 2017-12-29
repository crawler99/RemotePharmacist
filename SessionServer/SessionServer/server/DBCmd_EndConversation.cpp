#include "DBCmd_EndConversation.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_EndConversation::DBCmd_EndConversation(Poco::UInt32 userId)
        : _userId(userId)
    {
    }

    bool DBCmd_EndConversation::execute(PGconn *pConn)
    {
		// ----------------------------------------------------------------------
        std::stringstream ss;
        ss << "UPDATE tbl_Conversations SET f_stopping_time=NOW() WHERE f_stopping_time IS NULL AND f_pharmacist_id=" << _userId;

        _pResult = PQexec(pConn, ss.str().c_str());

        bool succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

		PQclear(_pResult);

        // ----------------------------------------------------------------------
        ss.str("");
        ss << "UPDATE tbl_Pharmacists_Stats SET f_status=" << USER_STATUS_IDLE
            << " WHERE f_id=" << _userId;

        _pResult = PQexec(pConn, ss.str().c_str());

        succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        PQclear(_pResult);
        return succ;
    }

    void DBCmd_EndConversation::reset()
    {   
    }

} }