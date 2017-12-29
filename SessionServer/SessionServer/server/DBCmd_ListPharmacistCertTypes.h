#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <vector>

namespace sserver { namespace server { 

    class DBCmd_ListPharmacistCertTypes : public DBCmd
    {
    public:
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        struct Item
        {
            Poco::UInt32  _id;
            std::string   _certTypeName;
        };
        const std::vector<Item>& getCertTypes();

    private:
        // execution result
        std::vector<Item> _resultItems;
        PGresult          *_pResult;
    };

} }