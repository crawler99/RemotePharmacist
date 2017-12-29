#include "DBCmd_ListDealsOfPrescriptionDrug.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server {

    DBCmd_ListDealsOfPrescriptionDrug::DBCmd_ListDealsOfPrescriptionDrug
        (Poco::UInt32 storeId,
        const std::string &startTime,
        const std::string &endTime) : _storeId(storeId), _startTime(startTime), _endTime(endTime)
    {
    }

    DBCmd_ListDealsOfPrescriptionDrug::~DBCmd_ListDealsOfPrescriptionDrug()
    {
        reset();
    }

    bool DBCmd_ListDealsOfPrescriptionDrug::execute(PGconn *pConn)
    {
        std::stringstream ss;
        ss << "SELECT * FROM tbl_DealOfPrescriptionDrug, tbl_Stores "
            << "WHERE tbl_DealOfPrescriptionDrug.f_store_id=" << _storeId 
            << " AND tbl_DealOfPrescriptionDrug.f_store_id=tbl_Stores.f_id "
            << " AND tbl_Stores.f_is_active=true" ;
        
        if (!_startTime.empty())
        {
            ss << " AND (f_time >= '" << _startTime << "')";
        }

        if (!_endTime.empty())
        {
            ss << " AND (f_time <= '" << _endTime << "')";
        }

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
            Item item;
            std::string valStr(PQgetvalue(_pResult, i, 0));
            succ = Utils::string2Num<Poco::UInt32>(valStr, item._dealId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get deal id from the DB command result");
                PQclear(_pResult);
                return succ;
            }

            item._time = PQgetvalue(_pResult,i,2);
            item._buyerName = PQgetvalue(_pResult,i,3);

            valStr = PQgetvalue(_pResult, i, 4);
            succ = Utils::string2Num<Poco::UInt32>(valStr, item._buyerAge);
            if (!succ)
            {
                LOG_DEBUG("Failed to get buyer age from the DB command result");
                PQclear(_pResult);
                return succ;
            }

            valStr = PQgetvalue(_pResult, i, 5);
            item._buyerIsMale = (valStr == "t");

            item._drugHuoHao = PQgetvalue(_pResult,i,6);
            item._drugMingCheng = PQgetvalue(_pResult,i,7);
            item._drugPiHao = PQgetvalue(_pResult,i,8);
            item._drugGuiGe = PQgetvalue(_pResult,i,9);
            item._drugJiLiang = PQgetvalue(_pResult,i,10);
            item._drugShengChanChangJia = PQgetvalue(_pResult,i,11);
            item._drugChuFangLaiYuan = PQgetvalue(_pResult,i,12);

            _resultItems.push_back(item);
        }

        PQclear(_pResult);
        return succ;
    }

    void DBCmd_ListDealsOfPrescriptionDrug::reset()
    {
        _resultItems.clear();
    }

    const std::vector<DBCmd_ListDealsOfPrescriptionDrug::Item>& DBCmd_ListDealsOfPrescriptionDrug::getItems()
    {
        return _resultItems;
    }

} }