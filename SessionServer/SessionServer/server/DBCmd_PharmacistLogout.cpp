#include "DBCmd_PharmacistLogout.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_PharmacistLogout::DBCmd_PharmacistLogout(Poco::UInt32 userId)
        : _userId(userId)
    {
    }

    bool DBCmd_PharmacistLogout::execute(PGconn *pConn)
    {
		// ----------------------------------------------------------------------
        std::stringstream ss;
        ss << "UPDATE tbl_Pharmacists_Stats SET f_status=" << USER_STATUS_OFFLINE
            << ", f_last_logout_time=NOW()"
            << " WHERE f_id=" << _userId;

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

    void DBCmd_PharmacistLogout::reset()
    {   
    }

} }