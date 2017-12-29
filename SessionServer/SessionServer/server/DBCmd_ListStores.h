#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <set>
#include <map>

namespace sserver { namespace server { 

    class DBCmd_ListStores : public DBCmd
    {
    public:
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        struct Item
        {
            std::string             _name;
			std::string             _addr;
            std::set<std::string>   _patientAccts;
        };
        const std::map<Poco::UInt32, Item>& getStores();

    private:
        // execution result
        std::map<Poco::UInt32, Item> _resultItems;
        PGresult                     *_pResult;
    };

} }