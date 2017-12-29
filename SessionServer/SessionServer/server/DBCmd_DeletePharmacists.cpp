#include "DBCmd_DeletePharmacists.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_DeletePharmacists::DBCmd_DeletePharmacists(const std::set<Poco::UInt32> &pharmacistIds) 
        : _pharmacistIds(pharmacistIds)
    {
        reset();
    }

    bool DBCmd_DeletePharmacists::execute(PGconn *pConn)
    {
        if (_pharmacistIds.empty()) return true;

        std::stringstream ss;
        ss << "UPDATE tbl_Users SET f_is_active=false WHERE f_id IN ("
            << "SELECT f_id FROM tbl_Pharmacists WHERE f_id IN(";

        std::set<Poco::UInt32>::const_iterator cit = _pharmacistIds.begin();
        ss << *cit;
        ++cit;

        while (cit != _pharmacistIds.end())
        {
            ss << ", " << *cit;
            ++cit;
        }
        ss << "));";

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

    void DBCmd_DeletePharmacists::reset()
    {   
    }

} }