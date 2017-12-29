#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include "Poco/LocalDateTime.h"

namespace sserver { namespace server { 

    class DBCmd_LogConversation : public DBCmd
    {
    public:
        DBCmd_LogConversation(Poco::UInt32 pharmacistId,
                              Poco::UInt32 pharmacistSessionId,
                              Poco::UInt32 patientId,
                              Poco::UInt32 patientSessionId,
							  Poco::LocalDateTime startTime);
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
		// ------------------------------------------------------------
		bool         isSuccess();
		Poco::UInt32 getConversationId();

    private:
        // input parameters
        Poco::UInt32         _pharmacistId;
        Poco::UInt32         _pharmacistSessionId;
        Poco::UInt32         _patientId;
        Poco::UInt32         _patientSessionId;
		Poco::LocalDateTime  _startTime;

        // execution result
		Poco::UInt32    _conversationId;
        PGresult        *_pResult;
    };

} }