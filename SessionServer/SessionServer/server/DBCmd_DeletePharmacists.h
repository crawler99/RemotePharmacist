#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <set>

namespace sserver { namespace server { 

    class DBCmd_DeletePharmacists : public DBCmd
    {
    public:
        DBCmd_DeletePharmacists(const std::set<Poco::UInt32> &pharmacistIds);
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();

    private:
        // input parameters
        std::set<Poco::UInt32>  _pharmacistIds;

        // execution result
        PGresult                *_pResult;
    };

} }