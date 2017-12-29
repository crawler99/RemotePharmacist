#include "DBCmd_PharmacistIdByStore.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

	DBCmd_PharmacistIdByStore::DBCmd_PharmacistIdByStore(Poco::UInt32 storeId, Poco::UInt32 certTypeId) 
		: _storeId(storeId), _certTypeId(certTypeId)
	{
		reset();
	}

	bool DBCmd_PharmacistIdByStore::execute(PGconn *pConn)
	{
		// ----------------------------------------------------------------------
		std::stringstream ss;
		ss << "SELECT tbl_Pharmacists.f_id "
			<< "FROM tbl_Users, tbl_Pharmacists, tbl_Stores, tbl_Stores_Pharmacists_Map "
			<< "WHERE (tbl_Stores.f_id=" << _storeId << ") "
			<< "AND (tbl_Stores_Pharmacists_Map.f_store_id=tbl_Stores.f_id) "
			<< "AND (tbl_Pharmacists.f_id=tbl_Stores_Pharmacists_Map.f_pharmacist_id) "
			<< "AND (tbl_Pharmacists.f_certificate_type_id=" << _certTypeId << ")"
            << "AND (tbl_Pharmacists.f_id=tbl_Users.f_id) "
            << "AND (tbl_Users.f_is_active=true)";

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
			Poco::UInt32 id;
			std::string valStr(PQgetvalue(_pResult, i, 0));
			succ = Utils::string2Num<Poco::UInt32>(valStr, id);
			if (!succ)
			{
				LOG_DEBUG("Failed to get pharmacist id from the DB command result");
				continue;
			}
			_pharmacistIds.insert(id);
		}

		PQclear(_pResult);
		return succ;
	}

	void DBCmd_PharmacistIdByStore::reset()
	{
		_pharmacistIds.clear();
	}

	const std::set<Poco::UInt32>& DBCmd_PharmacistIdByStore::getPharmacistIds()
	{
		return _pharmacistIds;
	}

} }