#include "DBCmd_SavePrescriptionLoc.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include "Poco/Path.h"
#include <sstream>

namespace sserver { namespace server { 

	DBCmd_SavePrescriptionLoc::DBCmd_SavePrescriptionLoc(Poco::UInt32 conversationId,
		                                                 MSG_DIRECTION direction,
		                                                 const std::string &relativeDir)
		: _conversationId(conversationId)
        , _direction(direction)
        , _relativeDir(relativeDir)
	{
		reset();
	}

	bool DBCmd_SavePrescriptionLoc::execute(PGconn *pConn)
	{
        if (_relativeDir.empty()) return false;

		// ----------------------------------------------------------------------
		std::stringstream ss;
		ss << "INSERT INTO tbl_Prescriptions (f_conversation_id, f_direction, f_gen_time, f_prescription_loc)"
			<< " VALUES (" << _conversationId << ", " << _direction << ", NOW(), '" 
            << _relativeDir << "') RETURNING f_id;";
		
        _pResult = PQexec(pConn, ss.str().c_str());

        bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

		std::string valStr(PQgetvalue(_pResult,0,0));
		succ = Utils::string2Num<Poco::UInt32>(valStr, _prescriptionId);
		if (!succ)
		{
			LOG_DEBUG("Failed to get prescription id from the DB command result");
			PQclear(_pResult);
			return succ;
		}

        PQclear(_pResult);

        // ----------------------------------------------------------------------
        ss.str("");
        ss << "SELECT tbl_Stores.f_id, tbl_Stores.f_name FROM tbl_Stores, tbl_Patients, tbl_Conversations, tbl_Prescriptions "
            << "WHERE tbl_Prescriptions.f_id=" << _prescriptionId 
            << " AND tbl_Prescriptions.f_conversation_id=tbl_Conversations.f_id"
            << " AND tbl_Conversations.f_patient_id=tbl_Patients.f_id"
            << " AND tbl_Patients.f_store_id=tbl_Stores.f_id";

        _pResult = PQexec(pConn, ss.str().c_str());

        succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }
    
        if (PQntuples(_pResult) > 0)
        {
            std::string valStr(PQgetvalue(_pResult,0,0));
            succ = Utils::string2Num<Poco::UInt32>(valStr, _storeId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get store id from the DB command result");
                PQclear(_pResult);
                return succ;
            }
            _storeName = PQgetvalue(_pResult, 0, 1);
        }

		PQclear(_pResult);

        // ----------------------------------------------------------------------
        ss.str("");
        ss << _relativeDir << Poco::Path::separator() << _storeId 
            << Poco::Path::separator() << _prescriptionId;
        std::string loc = ss.str();

        ss.str("");
        ss << "UPDATE tbl_Prescriptions SET f_prescription_loc='"
            << loc << "' WHERE f_id=" << _prescriptionId;

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

	bool DBCmd_SavePrescriptionLoc::isSuccess()
	{
		return (_prescriptionId != 0);
	}

    const std::string& DBCmd_SavePrescriptionLoc::getStoreName()
    {
        return _storeName;
    }

    Poco::UInt32 DBCmd_SavePrescriptionLoc::getStoreId()
    {
        return _storeId;
    }

    Poco::UInt32 DBCmd_SavePrescriptionLoc::getPrescriptionId()
    {
        return _prescriptionId;
    }

	void DBCmd_SavePrescriptionLoc::reset()
	{   
		_prescriptionId = 0;
	}

} }