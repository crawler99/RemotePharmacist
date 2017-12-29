#pragma once

#include "DBCmd.h"
#include "Poco/Foundation.h"

namespace sserver { namespace server { 

    class DBCmd_RefreshFingerprint : public DBCmd
    {
    public:
        DBCmd_RefreshFingerprint(Poco::UInt32 userId);
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