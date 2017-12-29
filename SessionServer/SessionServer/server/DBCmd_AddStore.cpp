#include "DBCmd_AddStore.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

	DBCmd_AddStore::DBCmd_AddStore(Poco::UInt32       updateStoreId,
                                   const std::string &name, 
		                           const std::string &addr,
		                           const std::set<Poco::UInt32> &pharmacistIds) 
		: _updateStoreId(updateStoreId)
        , _name(name)
        , _addr(addr)
        , _pharmacistIds(pharmacistIds)
	{
		reset();
	}

    bool DBCmd_AddStore::execute(PGconn *pConn)
    {
        if (_updateStoreId == 0)
            return _executeInsert(pConn);	
        else
            return _executeUpdate(pConn);
    }

	bool DBCmd_AddStore::_executeInsert(PGconn *pConn)
	{
		if (_name.empty() || _addr.empty()) return false;

		// ----------------------------------------------------------------------
		std::stringstream ss;
		ss << "SELECT f_id FROM tbl_Stores WHERE f_name='" << _name << "' AND f_is_active=false";
		_pResult = PQexec(pConn, ss.str().c_str());

		bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
		if (!succ)
		{
			LOG_DEBUG(PQresultErrorMessage(_pResult));
			PQclear(_pResult);
			return succ;
		}

		if (PQntuples(_pResult) != 0)
		{
			std::string valStr(PQgetvalue(_pResult, 0, 0));
			succ = Utils::string2Num<Poco::UInt32>(valStr, _insertStoreId);
			if (!succ)
			{
				LOG_DEBUG("Failed to get store id from the DB command result");
				PQclear(_pResult);
				return succ;
			}

			ss.str("");
			ss << "UPDATE tbl_Stores SET f_is_active=true WHERE f_id=" << _insertStoreId;
			
			_pResult = PQexec(pConn, ss.str().c_str());

			bool succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
			if (!succ)
			{
				LOG_DEBUG(PQresultErrorMessage(_pResult));
				PQclear(_pResult);
				return succ;
			}

			PQclear(_pResult);

			Poco::UInt32 old = _updateStoreId;
			_updateStoreId = _insertStoreId;
			succ = _executeUpdate(pConn);
			_updateStoreId = old;

			return succ;
		}

		// ----------------------------------------------------------------------
		ss.str("");
		ss << "INSERT INTO tbl_Stores VALUES (DEFAULT, '"
			<< _name << "', '" << _addr << "', true) RETURNING f_id";

		_pResult = PQexec(pConn, ss.str().c_str());

		succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
		if (!succ)
		{
			LOG_DEBUG(PQresultErrorMessage(_pResult));
			PQclear(_pResult);
			return succ;
		}

		std::string valStr(PQgetvalue(_pResult,0,0));
		succ = Utils::string2Num<Poco::UInt32>(valStr, _insertStoreId);
		if (!succ)
		{
			LOG_DEBUG("Failed to get store id from the DB command result");
			PQclear(_pResult);
			return succ;
		}

		PQclear(_pResult);

        // ----------------------------------------------------------------------
		std::set<Poco::UInt32>::iterator it = _pharmacistIds.begin();
		while (it != _pharmacistIds.end())
        {
			ss.str("");
			ss << "INSERT INTO tbl_Stores_Pharmacists_Map (f_store_id, f_pharmacist_id) "
				<< "VALUES (" << _insertStoreId << ", " << *it << ")";

			_pResult = PQexec(pConn, ss.str().c_str());

			succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
			if (!succ)
			{
				LOG_DEBUG(PQresultErrorMessage(_pResult));
				PQclear(_pResult);
				return succ;
			}

            PQclear(_pResult);
			++it;
        }
		
		return succ;
	}

    bool DBCmd_AddStore::_executeUpdate(PGconn *pConn)
    {
        if (_name.empty() || _addr.empty()) return false;

        // ----------------------------------------------------------------------
        std::stringstream ss;
        ss << "UPDATE tbl_Stores SET f_address='" << _addr
            << "' WHERE f_id=" << _updateStoreId;

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
        ss << "DELETE FROM tbl_Stores_Pharmacists_Map WHERE f_store_id=" << _updateStoreId;
        
        _pResult = PQexec(pConn, ss.str().c_str());

        succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        PQclear(_pResult);

        // ----------------------------------------------------------------------
        std::set<Poco::UInt32>::iterator it = _pharmacistIds.begin();
        while (it != _pharmacistIds.end())
        {
            ss.str("");
            ss << "INSERT INTO tbl_Stores_Pharmacists_Map (f_store_id, f_pharmacist_id) "
                << "VALUES (" << _updateStoreId << ", " << *it << ")";

            _pResult = PQexec(pConn, ss.str().c_str());

            succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
            if (!succ)
            {
                LOG_DEBUG(PQresultErrorMessage(_pResult));
                PQclear(_pResult);
                return succ;
            }

			PQclear(_pResult);
            ++it;
        }

        return succ;
    }

	bool DBCmd_AddStore::isSuccess()
	{
		return (getStoreId() != 0);
	}

	Poco::UInt32 DBCmd_AddStore::getStoreId()
	{
		return (_updateStoreId == 0 ? _insertStoreId : _updateStoreId);
	}

	void DBCmd_AddStore::reset()
	{   
		_insertStoreId = 0;
	}

} }