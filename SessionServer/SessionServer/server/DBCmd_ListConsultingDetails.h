#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <map>
#include <vector>

namespace sserver { namespace server { 

    class DBCmd_ListConsultingDetails : public DBCmd
    {
    public:
        DBCmd_ListConsultingDetails(Poco::UInt32 storeId,
                                    const std::string &startTime,
                                    const std::string &endTime);
        ~DBCmd_ListConsultingDetails();
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        struct Item
        {
            Poco::UInt32                _pharmacistId;
            Poco::UInt32                _patientId;
            std::string                 _startTime;
            std::string                 _stopTime;
            std::vector<std::string>    _prescriptionLocs;
            std::string                 _pharmacistVideoLoc;
            std::string                 _patientVideoLoc;
            std::string                 _pharmacistAudioLoc;
            std::string                 _patientAudioLoc;
        };
        const std::map<Poco::UInt32, Item>& getItems();

    private:
        // input parameters
        Poco::UInt32                 _storeId;
        std::string                  _startTime;
        std::string                  _endTime;

        // execution result
        std::map<Poco::UInt32, Item> _resultItems;
        PGresult                     *_pResult;
    };

} }