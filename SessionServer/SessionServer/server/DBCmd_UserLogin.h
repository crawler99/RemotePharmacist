#pragma once

#include "DBCmd.h"
#include "Poco/Foundation.h"

namespace sserver { namespace server { 

    class DBCmd_UserLogin : public DBCmd
    {
    public:
        DBCmd_UserLogin(const std::string &username, 
			            const std::string &password,
						void *pFingerprint, Poco::UInt32 fpLen);
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
		// ------------------------------------------------------------
		bool isSuccess();
        // ------------------------------------------------------------
        struct Item  
        {
            Poco::UInt32    _userId;
            Poco::UInt8     _userRole;
            std::string     _regTime;
            // (patient only)
            Poco::UInt32    _storeId;
            // (pharmacist only)
            std::string     _realName;
            Poco::UInt32    _certTypeId;
            Poco::UInt32    _fpLen;
            void            *_pFingerprint;
            Poco::UInt32    _signatureLen;
            void            *_pSignature;
        };
        const Item& getResultItem();

    private:
        // input parameters
        std::string     _username;
        std::string     _password;

		void            *_pFingerprint;
		Poco::UInt32    _fpLen;

        // execution result
        Item            _resultItem;		
        PGresult        *_pResult;
    };

} }