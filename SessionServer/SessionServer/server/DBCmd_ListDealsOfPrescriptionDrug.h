#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"
#include <vector>

namespace sserver { namespace server { 

    class DBCmd_ListDealsOfPrescriptionDrug : public DBCmd
    {
    public:
        DBCmd_ListDealsOfPrescriptionDrug(Poco::UInt32 storeId,
                                          const std::string &startTime,
                                          const std::string &endTime);
        ~DBCmd_ListDealsOfPrescriptionDrug();
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        struct Item
        {
            Poco::UInt32       _dealId;
            std::string        _time;
            std::string        _buyerName;
            Poco::UInt32       _buyerAge;
            bool               _buyerIsMale;
            std::string        _drugHuoHao;
            std::string        _drugMingCheng;
            std::string        _drugPiHao;
            std::string        _drugGuiGe;
            std::string        _drugJiLiang;
            std::string        _drugShengChanChangJia;
            std::string        _drugChuFangLaiYuan;
        };
        const std::vector<Item>& getItems();

    private:
        // input parameters
        Poco::UInt32      _storeId;
        std::string       _startTime;
        std::string       _endTime;

        // execution result
        std::vector<Item> _resultItems;
        PGresult          *_pResult;
    };

} }