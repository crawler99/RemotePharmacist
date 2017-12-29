#pragma once

#include "DBCmd.h"
#include "Poco/Foundation.h"
#include <vector>

namespace sserver { namespace server { 

    class DBCmd_PharmacistDetailByStore : public DBCmd
    {
    public:
        DBCmd_PharmacistDetailByStore(Poco::UInt32 storeId);
        ~DBCmd_PharmacistDetailByStore();
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        struct Item  
        {
            std::string   _realName;
			Poco::UInt32  _certTypeId;
            Poco::UInt32  _introductionLen;
            void          *_pIntroduction;
            Poco::UInt32  _photoLen;
            void          *_pPhoto;
        };
        const std::vector<Item>& getItems();

    private:
        // input parameters
        Poco::UInt32      _storeId;

        // execution result
        std::vector<Item> _resultItems;
        PGresult          *_pResult;
    };

} }