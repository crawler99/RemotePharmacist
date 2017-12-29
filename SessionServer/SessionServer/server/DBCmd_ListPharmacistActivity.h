#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <vector>

namespace sserver { namespace server { 

	class DBCmd_ListPharmacistActivity : public DBCmd
	{
	public:
		DBCmd_ListPharmacistActivity(Poco::UInt32 pharmacistId,
			                         const std::string &startTime,
			                         const std::string &endTime);
		~DBCmd_ListPharmacistActivity();
		// ------------------------------------------------------------
		bool execute(PGconn *pConn);
		void reset();
		// ------------------------------------------------------------
		const std::vector<std::string>& getItems();

	private:
		// input parameters
		Poco::UInt32             _pharmacistsId;
		std::string              _startTime;
		std::string              _endTime;

		// execution result
		std::vector<std::string> _resultItems;
		PGresult                 *_pResult;
	};

} }