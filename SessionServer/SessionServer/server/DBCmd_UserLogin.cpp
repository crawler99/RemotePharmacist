#include "DBCmd_UserLogin.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include "Enums.h"
#include "../3rd/finger/MyFinger.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_UserLogin::DBCmd_UserLogin(const std::string &username, 
		                             const std::string &password,
									 void *pFingerprint, Poco::UInt32 fpLen)
        : _username(username), _password(password)
		, _pFingerprint(pFingerprint), _fpLen(fpLen)
    {
		reset();
    }

    bool DBCmd_UserLogin::execute(PGconn *pConn)
    {
		// ----------------------------------------------------------------------
        std::stringstream ss;
		ss << "SELECT f_id, f_role, f_reg_time FROM tbl_Users WHERE f_username='" << _username 
            << "' AND f_password='" << Utils::genCredential(_password) 
            << "' AND f_is_active=true";
		
		_pResult = PQexec(pConn, ss.str().c_str());
		
		bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
		if (!succ)
		{
			LOG_ERROR(PQresultErrorMessage(_pResult));
			PQclear(_pResult);
			return succ;
		}

		if (PQntuples(_pResult) != 0)
		{
			// ----------------------------------------------------------------------
			std::string valStr(PQgetvalue(_pResult,0,0));
			succ = Utils::string2Num<Poco::UInt32>(valStr, _resultItem._userId);
			if (!succ)
			{
				LOG_ERROR("Failed to get user id from the DB command result");
				PQclear(_pResult);
				return succ;
			}

			valStr = PQgetvalue(_pResult,0,1);
			succ = Utils::string2Num<Poco::UInt8>(valStr, _resultItem._userRole);
			if (!succ)
			{
				LOG_ERROR("Failed to get user role from the DB command result");
				PQclear(_pResult);
				return succ;
			}

			_resultItem._regTime = PQgetvalue(_pResult,0,2);

			// ----------------------------------------------------------------------
			if (_resultItem._userRole != USER_PHARMACIST)
			{
				_resultItem._realName = "";
				if (_resultItem._userRole != USER_PATIENT)
				{
					return succ;
				}
			}

			// ----------------------------------------------------------------------
			if (_resultItem._userRole == USER_PATIENT)
			{
				ss.str("");
				ss << "SELECT f_store_id FROM tbl_Patients WHERE f_id=" << _resultItem._userId;

				_pResult = PQexec(pConn, ss.str().c_str());

				succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
				if (!succ)
				{
					LOG_ERROR(PQresultErrorMessage(_pResult));
					PQclear(_pResult);
					return succ;
				}

				valStr = PQgetvalue(_pResult,0,0);
				succ = Utils::string2Num<Poco::UInt32>(valStr, _resultItem._storeId);
				if (!succ)
				{
					LOG_ERROR("Failed to get store id from the DB command result");
					PQclear(_pResult);
					return succ;
				}
			}
		
			// ----------------------------------------------------------------------
			if (_resultItem._userRole == USER_PHARMACIST)
			{
				ss.str("");
				ss << "SELECT tbl_Pharmacists_Stats.f_status, tbl_Pharmacists.f_real_name, "
                    << "tbl_Pharmacists.f_certificate_type_id, tbl_Pharmacists.f_fingerprint, tbl_Pharmacists.f_signature "
                    << "FROM tbl_Pharmacists, tbl_Pharmacists_Stats WHERE tbl_Pharmacists.f_id=" << _resultItem._userId
                    << " AND tbl_Pharmacists_Stats.f_id=tbl_Pharmacists.f_id";

				PQclear(_pResult);
				_pResult = PQexec(pConn, ss.str().c_str());

				succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
				if (!succ)
				{
					LOG_ERROR(PQresultErrorMessage(_pResult));
					PQclear(_pResult);
					return succ;
				}

                valStr = PQgetvalue(_pResult,0,0);
                Poco::UInt8 status;
                succ = Utils::string2Num<Poco::UInt8>(valStr, status);
                if (!succ)
                {
                    LOG_ERROR("Failed to get user status from the DB command result");
                    PQclear(_pResult);
                    return succ;
                }

                if (status != USER_STATUS_OFFLINE)
                {
                    LOG_ERROR("User already logged in.");
                    PQclear(_pResult);
                    return false;
                }

				_resultItem._realName = PQgetvalue(_pResult,0,1);

                valStr = PQgetvalue(_pResult,0,2);
                succ = Utils::string2Num<Poco::UInt32>(valStr, _resultItem._certTypeId);
                if (!succ)
                {
                    LOG_ERROR("Failed to get certificate type id from the DB command result");
                    PQclear(_pResult);
                    return succ;
                }
                
                // store the fingerprint content in heap
                _resultItem._fpLen = PQgetlength(_pResult,0,3);
                if (_resultItem._fpLen != 0)
                {
                    char *pContent = PQgetvalue(_pResult,0,3);

                    // convert to binary
                    size_t len;
                    unsigned char *pTemp = PQunescapeBytea(reinterpret_cast<const unsigned char*>(pContent), &len);
                    // copy the result out
                    _resultItem._pFingerprint = new unsigned char [len];
                    memcpy(_resultItem._pFingerprint, pTemp, len);
                    _resultItem._fpLen = len;
                    PQfreemem(pTemp);
                }
                else
                {
                    _resultItem._pFingerprint = NULL;
                }
				
				// if the fingerprint passed in is not NULL, verify it
				if ((_pFingerprint != NULL) && (_fpLen > 0))
				{
					CFinger finger;
					if ((!finger.LoadLib()) || (!finger.ComMask(reinterpret_cast<unsigned char*>(_resultItem._pFingerprint),
						                                        512, 
																reinterpret_cast<unsigned char*>(_pFingerprint))))
					{
						LOG_ERROR("Fingerprint doesn't match, login failed");
						PQclear(_pResult);
						return false;
					}
				}

                // store the signature content in heap
                _resultItem._signatureLen = PQgetlength(_pResult,0,4);
                if (_resultItem._signatureLen != 0)
                {
                    char *pContent = PQgetvalue(_pResult,0,4);

                    // convert to binary
                    size_t len;
                    unsigned char *pTemp = PQunescapeBytea(reinterpret_cast<const unsigned char*>(pContent), &len);
                    // copy the result out
                    _resultItem._pSignature = new unsigned char [len];
                    memcpy(_resultItem._pSignature, pTemp, len);
                    _resultItem._signatureLen = len;
                    PQfreemem(pTemp);
                }
                else
                {
                    _resultItem._pSignature = NULL;
                }

				// ----------------------------------------------------------------------
				ss.str("");
				ss << "UPDATE tbl_Pharmacists_Stats SET f_status=" << USER_STATUS_IDLE 
					<< ", f_last_login_time=NOW()"<< " WHERE f_id=" << _resultItem._userId;

				PQclear(_pResult);
				_pResult = PQexec(pConn, ss.str().c_str());

				succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
				if (!succ)
				{
					LOG_DEBUG(PQresultErrorMessage(_pResult));
					PQclear(_pResult);
					return succ;
				}

				// ----------------------------------------------------------------------
				ss.str("");
				ss << "INSERT INTO tbl_Pharmacists_Activity_Log VALUES(" << _resultItem._userId
					<< ", NOW())";

				PQclear(_pResult);
				_pResult = PQexec(pConn, ss.str().c_str());

				succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
				if (!succ)
				{
					LOG_DEBUG(PQresultErrorMessage(_pResult));
					PQclear(_pResult);
					return succ;
				}
			}
  		}
		
		PQclear(_pResult);
        return succ;
    }

	bool DBCmd_UserLogin::isSuccess()
	{
		return _resultItem._userId != 0;
	}

    void DBCmd_UserLogin::reset()
    {
		_resultItem._userId = 0;
		_resultItem._userRole = 0;
        _resultItem._regTime = "";
        _resultItem._storeId = 0;
		_resultItem._realName = "";
        _resultItem._certTypeId = 0;
        _resultItem._fpLen = 0;
        _resultItem._pFingerprint = NULL;
		_resultItem._signatureLen = 0;
		_resultItem._pSignature = NULL;
    }

    const DBCmd_UserLogin::Item& DBCmd_UserLogin::getResultItem()
    {
        return _resultItem;
    }

} }