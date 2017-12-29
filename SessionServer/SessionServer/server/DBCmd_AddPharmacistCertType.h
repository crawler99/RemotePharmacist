#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <vector>

namespace sserver { namespace server { 

    class DBCmd_AddPharmacistCertType : public DBCmd
    {
    public:
        DBCmd_AddPharmacistCertType(const std::string &certTypeName);
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        bool              isSuccess();
        Poco::UInt32      getId();

    private:
        // input parameters
        std::string       _certTypeName;

        // execution result
        Poco::UInt32      _id;
        PGresult          *_pResult;
    };

} }