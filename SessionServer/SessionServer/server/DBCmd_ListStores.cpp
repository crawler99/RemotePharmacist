#include "DBCmd_ListStores.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    bool DBCmd_ListStores::execute(PGconn *pConn)
    {
		// first round, fill info except patient accounts
        // ------------------------------------------------------------------
        std::stringstream ss;
        ss << "SELECT f_id, f_name, f_address FROM tbl_Stores WHERE f_is_active=true";

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
            Poco::UInt32 storeId;
            std::string valStr(PQgetvalue(_pResult, i, 0));
            succ = Utils::string2Num<Poco::UInt32>(valStr, storeId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get store id from the DB command result");
                continue;
            }

            Item item;
            item._name = PQgetvalue(_pResult,i,1);
			item._addr = PQgetvalue(_pResult,i,2);

            _resultItems.insert(std::make_pair(storeId, item));
        }

        PQclear(_pResult);

        // second round, fill patient accounts
        // ------------------------------------------------------------------
        ss.str("");
        ss << "SELECT tbl_Users.f_username, tbl_Patients.f_store_id "
            << "FROM tbl_Users, tbl_Stores, tbl_Patients "
            << "WHERE tbl_Patients.f_store_id=tbl_Stores.f_id AND tbl_Patients.f_id=tbl_Users.f_id AND tbl_Stores.f_is_active=true";

        _pResult = PQexec(pConn, ss.str().c_str());

        succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        numOfItems = PQntuples(_pResult);
        for (unsigned int i = 0; i < numOfItems; ++i)
        {
            std::string username = PQgetvalue(_pResult,i,0);

            Poco::UInt32 storeId;
            std::string valStr(PQgetvalue(_pResult, i, 1));
            succ = Utils::string2Num<Poco::UInt32>(valStr, storeId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get store id from the DB command result");
                continue;
            }

            std::map<Poco::UInt32, Item>::iterator it = _resultItems.find(storeId);
            if (it != _resultItems.end())
            {
                it->second._patientAccts.insert(username);
            }
        }

        PQclear(_pResult);
        return succ;
    }

    void DBCmd_ListStores::reset()
    {
        _resultItems.clear();
    }

    const std::map<Poco::UInt32, DBCmd_ListStores::Item>& DBCmd_ListStores::getStores()
    {
        return _resultItems;
    }

} }