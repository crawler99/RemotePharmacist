#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <set>

namespace sserver { namespace server { 

	class DBCmd_AddStore : public DBCmd
	{
	public:
		DBCmd_AddStore(Poco::UInt32       updateStoreId,
                       const std::string &name, 
			           const std::string &addr,
					   const std::set<Poco::UInt32> &pharmacistIds);
		// ------------------------------------------------------------
		bool execute(PGconn *pConn);
		void reset();
		// ------------------------------------------------------------
		bool              isSuccess();
		Poco::UInt32      getStoreId();

	private:
        bool _executeInsert(PGconn *pConn);
        bool _executeUpdate(PGconn *pConn);

        // input parameters
        Poco::UInt32            _updateStoreId;
		std::string             _name;
		std::string             _addr;
		std::set<Poco::UInt32>  _pharmacistIds;

		// execution result
		Poco::UInt32            _insertStoreId;
		PGresult                *_pResult;
	};

} }