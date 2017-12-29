#include "DBCmd_DeleteStores.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

	DBCmd_DeleteStores::DBCmd_DeleteStores(const std::set<Poco::UInt32> &storeIds) 
		: _storeIds(storeIds)
	{
		reset();
	}

	bool DBCmd_DeleteStores::execute(PGconn *pConn)
	{
		if (_storeIds.empty()) return true;
		
		// ----------------------------------------------------------------------
		std::stringstream ss;
		ss << "UPDATE tbl_Stores SET f_is_active=false WHERE f_id IN (";

		std::set<Poco::UInt32>::const_iterator cit = _storeIds.begin();
		ss << *cit;
		++cit;
		
		while (cit != _storeIds.end())
		{
			ss << ", " << *cit;
			++cit;
		}
		ss << ")";

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
		ss << "UPDATE tbl_Users SET f_is_active=false WHERE f_id IN ("
			<< "SELECT f_id FROM tbl_Patients WHERE f_store_id IN (";

		cit = _storeIds.begin();
		ss << *cit;
		++cit;

		while (cit != _storeIds.end())
		{
			ss << ", " << *cit;
			++cit;
		}
		ss << "))";

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

	void DBCmd_DeleteStores::reset()
	{   
	}

} }