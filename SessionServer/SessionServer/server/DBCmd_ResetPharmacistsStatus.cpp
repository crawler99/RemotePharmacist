#include "DBCmd_ResetPharmacistsStatus.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server {

	bool DBCmd_ResetPharmacistsStatus::execute(PGconn *pConn)
	{
		std::stringstream ss;
		ss << "UPDATE tbl_Pharmacists_Stats SET f_status=" << USER_STATUS_OFFLINE;

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

	void DBCmd_ResetPharmacistsStatus::reset()
	{
	}

} }