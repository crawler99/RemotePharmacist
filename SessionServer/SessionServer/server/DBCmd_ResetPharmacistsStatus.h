#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <map>
#include <vector>

namespace sserver { namespace server { 

	class DBCmd_ResetPharmacistsStatus : public DBCmd
	{
	public:
		bool execute(PGconn *pConn);
		void reset();

	private:
		PGresult  *_pResult;
	};

} }