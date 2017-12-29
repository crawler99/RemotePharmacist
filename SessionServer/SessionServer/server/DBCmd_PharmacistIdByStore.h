#pragma once

#include "DBCmd.h"
#include "Poco/Foundation.h"
#include <set>

namespace sserver { namespace server { 

	class DBCmd_PharmacistIdByStore : public DBCmd
	{
	public:
		DBCmd_PharmacistIdByStore(Poco::UInt32 storeId, Poco::UInt32 certTypeId);
		// ------------------------------------------------------------
		bool execute(PGconn *pConn);
		void reset();
		// ------------------------------------------------------------
		const std::set<Poco::UInt32>& getPharmacistIds();

	private:
		// input parameters
		Poco::UInt32             _storeId;
		Poco::UInt32             _certTypeId;

		// execution result
		std::set<Poco::UInt32>   _pharmacistIds;
		PGresult                 *_pResult;
	};

} }