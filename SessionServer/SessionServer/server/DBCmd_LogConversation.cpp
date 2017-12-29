#include "DBCmd_LogConversation.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>
#include "Poco/DateTimeFormatter.h"
#include "Poco/Path.h"

namespace sserver { namespace server { 

    DBCmd_LogConversation::DBCmd_LogConversation(Poco::UInt32 pharmacistId,
                                                 Poco::UInt32 pharmacistSessionId,
                                                 Poco::UInt32 patientId,
                                                 Poco::UInt32 patientSessionId,
												 Poco::LocalDateTime startTime)
    : _pharmacistId(pharmacistId)
    , _pharmacistSessionId(pharmacistSessionId)
    , _patientId(patientId)
    , _patientSessionId(patientSessionId)
	, _startTime(startTime)
   
    {
		reset();
    }

    bool DBCmd_LogConversation::execute(PGconn *pConn)
    {
		// ----------------------------------------------------------------------
        std::stringstream ss;
		std::string date = Poco::DateTimeFormatter::format(_startTime, "%Y-%m-%d");
		std::string time = Poco::DateTimeFormatter::format(_startTime, "%H_%M_%S");
		ss << date << Poco::Path::separator() << _pharmacistSessionId << Poco::Path::separator() << time;
		std::string pharmacistVideoLoc = ss.str();
		std::string pharmacistAudioLoc = ss.str();

		ss.str("");
		ss << date << Poco::Path::separator() << _patientSessionId << Poco::Path::separator() << time;
		std::string patientVideoLoc = ss.str();
		std::string patientAudioLoc = ss.str();

		ss.str("");
        ss << "INSERT INTO tbl_Conversations VALUES (DEFAULT, "
            << _pharmacistId << ", " << _pharmacistSessionId << ", "
            << _patientId << ", " << _patientSessionId << ", '"
			<< Poco::DateTimeFormatter::format(_startTime, "%Y-%m-%d %H:%M:%S") << "', NULL, '"
			<< pharmacistVideoLoc << "', '" << patientVideoLoc << "', '"
            << pharmacistAudioLoc << "', '" << patientAudioLoc << "') RETURNING f_id";

        _pResult = PQexec(pConn, ss.str().c_str());

		bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
		if (!succ)
		{
			LOG_DEBUG(PQresultErrorMessage(_pResult));
			PQclear(_pResult);
			return succ;
		}

		std::string valStr(PQgetvalue(_pResult,0,0));
		succ = Utils::string2Num<Poco::UInt32>(valStr, _conversationId);
		if (!succ)
		{
			LOG_DEBUG("Failed to get conversation id from the DB command result");
			PQclear(_pResult);
			return succ;
		}

		PQclear(_pResult);

        // ----------------------------------------------------------------------
        ss.str("");
        ss << "UPDATE tbl_Pharmacists_Stats SET f_status=" << USER_STATUS_BUSY
            << ", f_services_today = (SELECT CASE WHEN (date_trunc('day', NOW()) = date_trunc('day', f_last_login_time)) "
            << "THEN f_services_today + 1 ELSE 0 END FROM tbl_Pharmacists_Stats WHERE f_id=" << _pharmacistId << ")"
			<< ", f_services_total = f_services_total + 1 WHERE f_id=" << _pharmacistId;

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

    void DBCmd_LogConversation::reset()
    {   
		_conversationId = 0;
    }

	bool DBCmd_LogConversation::isSuccess()
	{
		return (_conversationId != 0);
	}

	Poco::UInt32 DBCmd_LogConversation::getConversationId()
	{
		return _conversationId;
	}

} }