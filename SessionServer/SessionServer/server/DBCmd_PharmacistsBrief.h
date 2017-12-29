#pragma once

#include "DBCmd.h"
#include "Poco/Foundation.h"
#include <vector>

namespace sserver { namespace server { 

	class DBCmd_PharmacistsBrief : public DBCmd
	{
	public:
		DBCmd_PharmacistsBrief(bool includePhoto);
		~DBCmd_PharmacistsBrief();
        // ------------------------------------------------------------
		bool execute(PGconn *pConn);
		void reset();
		// ------------------------------------------------------------
		struct Item  
		{
			Poco::UInt32  _userId;
			std::string   _realName;
            Poco::UInt32  _certTypeId;
			Poco::UInt8   _status;
			Poco::UInt32  _photoLen;
			void          *_pPhoto;
		};
		const std::vector<Item>& getItems();

	private:
		bool              _includePhoto;

		// execution result
		std::vector<Item> _resultItems;
		PGresult          *_pResult;
	};

} }