#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"

namespace sserver { namespace server { 

    class DBCmd_PharmacistLogout : public DBCmd
    {
    public:
        DBCmd_PharmacistLogout(Poco::UInt32 userId);
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();

    private:
        // input parameters
        Poco::UInt32    _userId;

        // execution result
        PGresult        *_pResult;
    };

} }