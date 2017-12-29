#include "DBCmd_ListPharmacistCertTypes.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    bool DBCmd_ListPharmacistCertTypes::execute(PGconn *pConn)
    {
		// ----------------------------------------------------------------------
        std::stringstream ss;
        ss << "SELECT f_id, f_type_name FROM tbl_Pharmacist_Cert_Types";

        _pResult = PQexec(pConn, ss.str().c_str());

        bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        int numOfItems = PQntuples(_pResult);
        for (unsigned int i = 0; i < numOfItems; ++i)
        {
            Item item;
            std::string valStr(PQgetvalue(_pResult, i, 0));
            succ = Utils::string2Num<Poco::UInt32>(valStr, item._id);
            if (!succ)
            {
                LOG_DEBUG("Failed to get certificate type id from the DB command result");
                continue;
            }

            item._certTypeName = PQgetvalue(_pResult,i,1);
            _resultItems.push_back(item);
        }

        PQclear(_pResult);
        return succ;
    }

    void DBCmd_ListPharmacistCertTypes::reset()
    {
        _resultItems.clear();
    }

    const std::vector<DBCmd_ListPharmacistCertTypes::Item>& 
        DBCmd_ListPharmacistCertTypes::getCertTypes()
    {
        return _resultItems;
    }

} }