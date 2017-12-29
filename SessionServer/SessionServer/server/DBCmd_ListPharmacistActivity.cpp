#include "DBCmd_ListPharmacistActivity.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server {

	DBCmd_ListPharmacistActivity::DBCmd_ListPharmacistActivity
		(Poco::UInt32 pharmacistId,
		const std::string &startTime,
		const std::string &endTime) : _pharmacistsId(pharmacistId), _startTime(startTime), _endTime(endTime)
	{
	}

	DBCmd_ListPharmacistActivity::~DBCmd_ListPharmacistActivity()
	{
		reset();
	}

	bool DBCmd_ListPharmacistActivity::execute(PGconn *pConn)
	{
		std::stringstream ss;
		ss << "SELECT tbl_Pharmacists_Activity_Log.f_log_time FROM tbl_Pharmacists_Activity_Log, tbl_Users "
			<< "WHERE tbl_Pharmacists_Activity_Log.f_id=" << _pharmacistsId 
			<< " AND tbl_Pharmacists_Activity_Log.f_id=tbl_Users.f_id "
			<< " AND tbl_Users.f_is_active=true" ;

		if (!_startTime.empty())
		{
			ss << " AND (tbl_Pharmacists_Activity_Log.f_log_time >= '" << _startTime << "')";
		}

		if (!_endTime.empty())
		{
			ss << " AND (tbl_Pharmacists_Activity_Log.f_log_time <= '" << _endTime << "')";
		}

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
			_resultItems.push_back(PQgetvalue(_pResult,i,0));
		}

		PQclear(_pResult);
		return succ;
	}

	void DBCmd_ListPharmacistActivity::reset()
	{
		_resultItems.clear();
	}

	const std::vector<std::string>& DBCmd_ListPharmacistActivity::getItems()
	{
		return _resultItems;
	}

} }