#pragma once

#include "DBCmd.h"
#include "Poco/Foundation.h"
#include <vector>
#include <set>

namespace sserver { namespace server { 

    class DBCmd_PharmacistsDetail : public DBCmd
    {
    public:
		DBCmd_PharmacistsDetail(Poco::UInt32 userId);
		~DBCmd_PharmacistsDetail();
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        struct Item  
        {
            Poco::UInt32             _userId;
            std::string              _username;
            std::string              _shenfenzheng;
            std::string              _realName;
            Poco::UInt32             _certTypeId;
            Poco::UInt8              _status;
			std::set<Poco::UInt32>   _coveredStoreIds;
            Poco::UInt32             _certLen;
            void                     *_pCert;
            Poco::UInt32             _fpLen;
            void                     *_pFingerprint;
            Poco::UInt32             _signatureLen;
            void                     *_pSignature;
            Poco::UInt32             _introductionLen;
            void                     *_pIntroduction;
            Poco::UInt32             _photoLen;
            void                     *_pPhoto;
        };
        const std::vector<Item>& getItems();

    private:
		// input parameters
		Poco::UInt32      _userId;  // 0 means get all pharmacists' detail 

        // execution result
        std::vector<Item> _resultItems;
        PGresult          *_pResult;
    };

} }