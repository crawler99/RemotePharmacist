#include "DBCmd_PatientAdd.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_PatientAdd::DBCmd_PatientAdd(bool isUpdate,
                                       const std::string &username, 
		                               const std::string &password,
		                               Poco::UInt32 storeId)
        : _isUpdate(isUpdate), _username(username), _password(password), _storeId(storeId)
    {
        reset();
    }

    bool DBCmd_PatientAdd::execute(PGconn *pConn)
    {
        return _isUpdate ? _executeUpdate(pConn) 
                         : _executeInsert(pConn);
    }

    bool DBCmd_PatientAdd::_executeInsert(PGconn *pConn)
    {
        // ----------------------------------------------------------------------
        std::stringstream ss;
		ss << "SELECT f_id FROM tbl_Users WHERE f_username='" << _username << "' AND "
			<< "f_is_active=false";
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
			succ = Utils::string2Num<Poco::UInt32>(valStr, _userId);
			if (!succ)
			{
				LOG_DEBUG("Failed to get user id from the DB command result");
				PQclear(_pResult);
				return succ;
			}

			ss.str("");
			ss << "UPDATE tbl_Users SET f_is_active=true WHERE f_id=" << _userId;
			_pResult = PQexec(pConn, ss.str().c_str());

			bool succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
			if (!succ)
			{
				LOG_DEBUG(PQresultErrorMessage(_pResult));
				PQclear(_pResult);
				return succ;
			}

			PQclear(_pResult);
			return _executeUpdate(pConn);
		}

		// ----------------------------------------------------------------------
		ss.str("");
        ss << "INSERT INTO tbl_Users VALUES (DEFAULT, '" 
            << _username << "', '" << Utils::genCredential(_password) << "', " << USER_PATIENT 
			<< ", NOW(), true) RETURNING f_id";

        _pResult = PQexec(pConn, ss.str().c_str());

        succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

		std::string valStr(PQgetvalue(_pResult,0,0));
		succ = Utils::string2Num<Poco::UInt32>(valStr, _userId);
		if (!succ)
		{
			LOG_DEBUG("Failed to get user id from the DB command result");
			PQclear(_pResult);
			return succ;
		}

		PQclear(_pResult);

        // ----------------------------------------------------------------------
        ss.str("");
        ss << "INSERT INTO tbl_Patients (f_id, f_store_id) "
			<< "VALUES (" << _userId << ", " << _storeId << ")";
		
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

    bool DBCmd_PatientAdd::_executeUpdate(PGconn *pConn)
    {
		// ------------------------------------------------------------------------
        std::stringstream ss;
        ss << "UPDATE tbl_Users SET f_password='" 
            << Utils::genCredential(_password) << "' WHERE f_username='" << _username 
            << "' RETURNING f_id";

        _pResult = PQexec(pConn, ss.str().c_str());

        bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }
		
		if (PQntuples(_pResult) > 0)
        {
            std::string valStr(PQgetvalue(_pResult,0,0));
            succ = Utils::string2Num<Poco::UInt32>(valStr, _userId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get user id from the DB command result");
                PQclear(_pResult);
                return succ;
            }

			PQclear(_pResult);

			// ------------------------------------------------------------------------
			ss.str("");
			ss << "UPDATE tbl_Patients SET f_store_id=" << _storeId 
				<< " WHERE f_id=" << _userId;
			_pResult = PQexec(pConn, ss.str().c_str());

			succ = (PGRES_COMMAND_OK == PQresultStatus(_pResult));
			if (!succ)
			{
				LOG_DEBUG(PQresultErrorMessage(_pResult));
				PQclear(_pResult);
				return succ;
			}
        }

        PQclear(_pResult);
        return succ;
    }

    bool DBCmd_PatientAdd::isSuccess()
    {
        return (_userId != 0);
    }

    Poco::UInt32 DBCmd_PatientAdd::getUserId()
    {
        return _userId;
    }

    void DBCmd_PatientAdd::reset()
    {   
        _userId = 0;
    }

} }