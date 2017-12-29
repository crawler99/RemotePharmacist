#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <set>

namespace sserver { namespace server { 

	class DBCmd_DeleteStores : public DBCmd
	{
	public:
		DBCmd_DeleteStores(const std::set<Poco::UInt32> &storeIds);
		// ------------------------------------------------------------
		bool execute(PGconn *pConn);
		void reset();
		
	private:
		// input parameters
		std::set<Poco::UInt32>  _storeIds;

		// execution result
		PGresult                *_pResult;
	};

} }