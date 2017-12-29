#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"

namespace sserver { namespace server { 

	class DBCmd_SavePrescriptionLoc : public DBCmd
	{
	public:
		DBCmd_SavePrescriptionLoc(Poco::UInt32 conversationId,
							      MSG_DIRECTION direction,
                                  const std::string &relativeDir);
		// ------------------------------------------------------------
		bool execute(PGconn *pConn);
		void reset();
		// ------------------------------------------------------------
		bool                isSuccess();
        Poco::UInt32        getStoreId();
        const std::string&  getStoreName();
        Poco::UInt32        getPrescriptionId();

	private:
		// input parameters
		Poco::UInt32    _conversationId;
		MSG_DIRECTION   _direction;
        std::string     _relativeDir;
		
		// execution result
		Poco::UInt32    _prescriptionId;
        Poco::UInt32    _storeId;
        std::string     _storeName;
		PGresult        *_pResult;
	};

} }