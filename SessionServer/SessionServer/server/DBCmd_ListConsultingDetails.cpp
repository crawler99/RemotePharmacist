#include "DBCmd_ListConsultingDetails.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>
#include "Poco/StringTokenizer.h"

namespace sserver { namespace server {

    DBCmd_ListConsultingDetails::DBCmd_ListConsultingDetails
        (Poco::UInt32 storeId,
        const std::string &startTime,
        const std::string &endTime) : _storeId(storeId), _startTime(startTime), _endTime(endTime)
    {
    }

    DBCmd_ListConsultingDetails::~DBCmd_ListConsultingDetails()
    {
        reset();
    }

    bool DBCmd_ListConsultingDetails::execute(PGconn *pConn)
    {
        // ----------------------------------------------------------------------------------------------------------------
        std::stringstream ss;
        ss << "SELECT tbl_Conversations.f_id, f_pharmacist_id, f_patient_id, f_starting_time, f_stopping_time, "
            << "f_pharmacist_video_loc, f_patient_video_loc, f_pharmacist_audio_loc, f_patient_audio_loc, "
			<< "sumtext(tbl_Prescriptions.f_prescription_loc ORDER BY tbl_Prescriptions.f_id) "
            << "FROM tbl_Conversations JOIN tbl_Patients ON tbl_Conversations.f_patient_id=tbl_Patients.f_id LEFT JOIN tbl_Prescriptions ON tbl_Conversations.f_id=tbl_Prescriptions.f_conversation_id "
            << "WHERE tbl_Patients.f_store_id=" << _storeId;
        
		if (!_startTime.empty())
		{
			ss << " AND (tbl_Conversations.f_starting_time >= '" << _startTime << "')";
		}

		if (!_endTime.empty())
		{
			ss << " AND (tbl_Conversations.f_stopping_time <= '" << _endTime << "')";
		}
		
		ss << " GROUP BY tbl_Conversations.f_id";

        _pResult = PQexec(pConn, ss.str().c_str());

        bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        int numOfItems = PQntuples(_pResult);
        for (unsigned int i = 0; i < numOfItems; ++i)
        {
            Poco::UInt32 conversationId;
            std::string valStr(PQgetvalue(_pResult, i, 0));
            succ = Utils::string2Num<Poco::UInt32>(valStr, conversationId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get conversation id from the DB command result");
                PQclear(_pResult);
                return succ;
            }

            Item item;
            valStr = PQgetvalue(_pResult, i, 1);
            succ = Utils::string2Num<Poco::UInt32>(valStr, item._pharmacistId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get pharmacist id from the DB command result");
                PQclear(_pResult);
                return succ;
            }

            valStr = (PQgetvalue(_pResult, i, 2));
            succ = Utils::string2Num<Poco::UInt32>(valStr, item._patientId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get patient id from the DB command result");
                PQclear(_pResult);
                return succ;
            }

            item._startTime = PQgetvalue(_pResult,i,3);
            item._stopTime = PQgetvalue(_pResult,i,4);
            item._pharmacistVideoLoc = PQgetvalue(_pResult,i,5);
            item._patientVideoLoc = PQgetvalue(_pResult,i,6);
            item._pharmacistAudioLoc = PQgetvalue(_pResult,i,7);
            item._patientAudioLoc = PQgetvalue(_pResult,i,8);

			valStr = PQgetvalue(_pResult,i,9);
			Poco::StringTokenizer tkz(valStr, DB_AGGREGATE_SEPARATOR, Poco::StringTokenizer::TOK_TRIM);
			Poco::StringTokenizer::Iterator tkIt = tkz.begin();
			while (tkIt != tkz.end())
			{
				item._prescriptionLocs.push_back(*tkIt);
				++tkIt;
			}

            _resultItems.insert(std::make_pair(conversationId, item));
        }

        PQclear(_pResult);
        return succ;
    }

    void DBCmd_ListConsultingDetails::reset()
    {
        _resultItems.clear();
    }

    const std::map<Poco::UInt32, DBCmd_ListConsultingDetails::Item>& DBCmd_ListConsultingDetails::getItems()
    {
        return _resultItems;
    }

} }