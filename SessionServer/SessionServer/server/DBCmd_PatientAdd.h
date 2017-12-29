#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"

namespace sserver { namespace server { 

    class DBCmd_PatientAdd : public DBCmd
    {
    public:
        DBCmd_PatientAdd(bool isUpdate,
                         const std::string &username, 
			             const std::string &password,
			             Poco::UInt32 storeId);
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        bool            isSuccess();
        Poco::UInt32    getUserId();

    private:
        bool            _executeInsert(PGconn *pConn);
        bool            _executeUpdate(PGconn *pConn);

        // input parameters
        bool            _isUpdate;
        std::string     _username;
        std::string     _password;
		Poco::UInt32    _storeId;
        
        // execution result
		Poco::UInt32    _userId;
        PGresult        *_pResult;
    };

} }