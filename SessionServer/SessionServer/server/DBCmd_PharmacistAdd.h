#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"

namespace sserver { namespace server { 

    class DBCmd_PharmacistAdd : public DBCmd
    {
    public:
        DBCmd_PharmacistAdd(Poco::UInt32 updatePharmacistId,
                            const std::string &username, 
			                const std::string &password,
							const std::string &shenfenzheng,
			                const std::string &realname,
                            Poco::UInt32 certTypeId,
                            void *pCert, Poco::UInt32 certLen,              /* pCert should be released by outside */
                            void *pFingerprint, Poco::UInt32 fpLen,         /* pFingerprint should be released by outside */
                            void *pSignature, Poco::UInt32 signatureLen,    /* pSignature should be released by outside */
			                const char *pIntro, Poco::UInt32 introLen,      /* pIntro should be released by outside */
			                void *pPhoto, Poco::UInt32 photoLen             /* pPhoto should be released by outside */ );
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        bool            isSuccess();
        Poco::UInt32    getPharmacistId();

    private:
        bool            _executeInsert(PGconn *pConn);
        bool            _executeUpdate(PGconn *pConn, bool overwritePassword);

        // input parameters
        Poco::UInt32    _updatePharmacistId;
        std::string     _username;
        std::string     _password;
		std::string     _shenfenzheng;
        std::string     _realname;
        Poco::UInt32    _certTypeId;

        void            *_pCert;
        Poco::UInt32    _certLen;

        void            *_pFingerprint;
        Poco::UInt32    _fpLen;

        void            *_pSignature;
        Poco::UInt32    _signatureLen;

		const char      *_pIntro;
		Poco::UInt32    _introLen;

		void            *_pPhoto;
		Poco::UInt32    _photoLen;

        // execution result
		Poco::UInt32    _insertPharmacistId;
        PGresult        *_pResult;
    };

} }