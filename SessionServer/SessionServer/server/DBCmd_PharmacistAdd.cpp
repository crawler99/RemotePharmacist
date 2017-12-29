#include "DBCmd_PharmacistAdd.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_PharmacistAdd::DBCmd_PharmacistAdd(Poco::UInt32 updatePharmacistId,
                                             const std::string &username, 
		                                     const std::string &password,
											 const std::string &shenfenzheng,
											 const std::string &realname,
                                             Poco::UInt32 certTypeId,
                                             void *pCert, Poco::UInt32 certLen,
                                             void *pFingerprint, Poco::UInt32 fpLen,
                                             void *pSignature, Poco::UInt32 signatureLen,
		                                     const char *pIntro, Poco::UInt32 introLen,
		                                     void *pPhoto, Poco::UInt32 photoLen)
        : _updatePharmacistId(updatePharmacistId)
        , _username(username), _password(password) 
		, _shenfenzheng(shenfenzheng), _realname(realname), _certTypeId(certTypeId)
        , _pCert(pCert), _certLen(certLen)
        , _pFingerprint(pFingerprint), _fpLen(fpLen)
        , _pSignature(pSignature), _signatureLen(signatureLen)
		, _pIntro(pIntro), _introLen(introLen)
		, _pPhoto(pPhoto), _photoLen(photoLen)
    {
        reset();
    }

    bool DBCmd_PharmacistAdd::execute(PGconn *pConn)
    {
        return (_updatePharmacistId == 0) ? _executeInsert(pConn) 
                                          : _executeUpdate(pConn, !_password.empty());
    }

    bool DBCmd_PharmacistAdd::_executeInsert(PGconn *pConn)
    {
		// ----------------------------------------------------------------------
		std::stringstream ss;
		ss << "SELECT tbl_Pharmacists.f_id FROM tbl_Users, tbl_Pharmacists "
            << "WHERE tbl_Pharmacists.f_id=tbl_Users.f_id AND tbl_Pharmacists.f_shenfenzheng='" 
            << _shenfenzheng << "' AND tbl_Users.f_is_active=false";
		_pResult = PQexec(pConn, ss.str().c_str());

		bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
		if (!succ)
		{
			LOG_DEBUG(PQresultErrorMessage(_pResult));
			PQclear(_pResult);
			return succ;
		}

		if (PQntuples(_pResult) != 0)
		{
			std::string valStr(PQgetvalue(_pResult, 0, 0));
			succ = Utils::string2Num<Poco::UInt32>(valStr, _insertPharmacistId);
			if (!succ)
			{
				LOG_DEBUG("Failed to get pharmacist id from the DB command result");
				PQclear(_pResult);
				return succ;
			}

			ss.str("");
			ss << "UPDATE tbl_Users SET f_is_active=true WHERE f_id=" << _insertPharmacistId;
			_pResult = PQexec(pConn, ss.str().c_str());

			bool succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
			if (!succ)
			{
				LOG_DEBUG(PQresultErrorMessage(_pResult));
				PQclear(_pResult);
				return succ;
			}

			PQclear(_pResult);

			Poco::UInt32 old = _updatePharmacistId;
			_updatePharmacistId = _insertPharmacistId;
			succ = _executeUpdate(pConn, true);
			_updatePharmacistId = old;

			return succ;
		}

		// ----------------------------------------------------------------------
        ss.str("");
        ss << "INSERT INTO tbl_Users VALUES (DEFAULT, '" 
            << _username << "', '" << Utils::genCredential(_password) 
            << "', " << USER_PHARMACIST << ", NOW(), true) RETURNING f_id";

        _pResult = PQexec(pConn, ss.str().c_str());

        succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        std::string valStr(PQgetvalue(_pResult,0,0));
        succ = Utils::string2Num<Poco::UInt32>(valStr, _insertPharmacistId);
        if (!succ)
        {
            LOG_DEBUG("Failed to get user id from the DB command result");
            PQclear(_pResult);
            return succ;
        }

		PQclear(_pResult);

        // ----------------------------------------------------------------------
        unsigned int numOfParams = 3;
        ss.str("");
        ss << "INSERT INTO tbl_Pharmacists (f_id, f_shenfenzheng, f_real_name, f_certificate_type_id"
            << ", f_certificate, f_fingerprint, f_signature"
            << (_introLen == 0 ? "" : ", f_intro")
            << (_photoLen == 0 ? "" : ", f_photo")
			<< ") VALUES (" << _insertPharmacistId << ", '" 
			<< _shenfenzheng << "', '"<< _realname << "', " << _certTypeId
            << ", $1::bytea, $2::bytea, $3::bytea";
		if (_introLen != 0)
		{
			ss << ", '";
			ss.write(_pIntro, _introLen);
			ss << "'";
		}
        if (_photoLen != 0)
        {
            ss << ", $4::bytea";
            ++ numOfParams;
        }
		ss << ");";

        // writing binary data should use a different API function
        const char * const paramValues[] = { reinterpret_cast<char*>(_pCert),
                                             reinterpret_cast<char*>(_pFingerprint),
                                             reinterpret_cast<char*>(_pSignature),
                                             reinterpret_cast<char*>(_pPhoto) };
        const int paramLenghts[] = { _certLen, _fpLen, _signatureLen, _photoLen };
        const int paramFormats[] = {1, 1, 1, 1};
        const int resultFormat = 0;
        _pResult = PQexecParams(
                        pConn,
                        ss.str().c_str(),
                        numOfParams,
                        NULL,
                        paramValues,
                        paramLenghts,
                        paramFormats,
                        resultFormat);

        succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
			PQclear(_pResult);
            return succ;
        }

		PQclear(_pResult);

		// ----------------------------------------------------------------------
		ss.str("");
		ss << "INSERT INTO tbl_Pharmacists_Stats VALUES(" << _insertPharmacistId << ","
			<< USER_STATUS_OFFLINE << ",NULL,NULL,0,0)";

		_pResult = PQexec(pConn, ss.str().c_str());

		succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
		if (!succ)
		{
			LOG_DEBUG(PQresultErrorMessage(_pResult));
			PQclear(_pResult);
			return succ;
		}

		PQclear(_pResult);
		return succ;
    }

    bool DBCmd_PharmacistAdd::_executeUpdate(PGconn *pConn, bool overwritePassword)
    {
        // ----------------------------------------------------------------------
        std::stringstream ss;
        if (overwritePassword)
        {
            ss << "UPDATE tbl_Users SET f_password='" << Utils::genCredential(_password) 
                << "' WHERE f_id=" << _updatePharmacistId;

            _pResult = PQexec(pConn, ss.str().c_str());

            bool succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
            if (!succ)
            {
                LOG_DEBUG(PQresultErrorMessage(_pResult));
                PQclear(_pResult);
                return succ;
            }

            PQclear(_pResult);
        }

        // ----------------------------------------------------------------------
        unsigned int numOfParams = 4;
        ss.str("");
        ss << "UPDATE tbl_Pharmacists SET f_shenfenzheng='" << _shenfenzheng << "', "
			<< "f_real_name='" << _realname << "', "
            << "f_certificate_type_id=" << _certTypeId << ", "
            << "f_certificate=$1::bytea, f_fingerprint=$2::bytea, f_signature=$3::bytea, "
            << "f_intro='" + (_introLen == 0 ? "" : std::string(_pIntro, _introLen)) + "', "
            << "f_photo=$4::bytea " 
            << "WHERE f_id=" << _updatePharmacistId;

        // writing binary data should use a different API function
        const char * const paramValues[] = { reinterpret_cast<char*>(_pCert),
                                             reinterpret_cast<char*>(_pFingerprint),
                                             reinterpret_cast<char*>(_pSignature),
                                             reinterpret_cast<char*>(_pPhoto) };
        const int paramLenghts[] = { _certLen, _fpLen, _signatureLen, _photoLen };
        const int paramFormats[] = {1, 1, 1, 1};
        const int resultFormat = 0;
        _pResult = PQexecParams(
                        pConn,
                        ss.str().c_str(),
                        numOfParams,
                        NULL,
                        paramValues,
                        paramLenghts,
                        paramFormats,
                        resultFormat);        

        bool succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        PQclear(_pResult);
        return succ;
    }

    bool DBCmd_PharmacistAdd::isSuccess()
    {
        return (getPharmacistId() != 0);
    }

    Poco::UInt32 DBCmd_PharmacistAdd::getPharmacistId()
    {
        return (_updatePharmacistId == 0 ? _insertPharmacistId : _updatePharmacistId);
    }

    void DBCmd_PharmacistAdd::reset()
    {   
        _insertPharmacistId = 0;
    }

} }