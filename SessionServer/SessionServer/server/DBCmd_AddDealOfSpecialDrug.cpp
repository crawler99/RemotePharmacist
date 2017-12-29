#include "DBCmd_AddDealOfSpecialDrug.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_AddDealOfSpecialDrug::DBCmd_AddDealOfSpecialDrug
        (Poco::UInt32      updateDealId,
        Poco::UInt32       storeId,
        const std::string &buyerName,
        const std::string &buyerShenFenZheng,
        Poco::UInt32       buyerAge,
        bool               buyerIsMale,
        const std::string &drugHuoHao,
        const std::string &drugMingCheng,
        const std::string &drugPiHao,
        const std::string &drugGuiGe,
        const std::string &drugJiLiang,
        const std::string &drugGouMaiShuLiang,
        const std::string &drugShengChanChangJia,
        const std::string &drugChuFangLaiYuan)
        : _updateDealId(updateDealId), _storeId(storeId)
        , _buyerName(buyerName), _buyerShenFenZheng(buyerShenFenZheng), _buyerAge(buyerAge), _buyerIsMale(buyerIsMale)
        , _drugHuoHao(drugHuoHao), _drugMingCheng(drugMingCheng), _drugPiHao(drugPiHao)
        , _drugGuiGe(drugGuiGe), _drugJiLiang(drugJiLiang), _drugGouMaiShuLiang(drugGouMaiShuLiang)
        , _drugShengChanChangJia(drugShengChanChangJia), _drugChuFangLaiYuan(drugChuFangLaiYuan)
    {
        reset();
    }

    bool DBCmd_AddDealOfSpecialDrug::execute(PGconn *pConn)
    {
        if (_updateDealId == 0)
            return _executeInsert(pConn);
        else
            return _executeUpdate(pConn);
    }

    bool DBCmd_AddDealOfSpecialDrug::_executeInsert(PGconn *pConn)
    {
        // ----------------------------------------------------------------------
        std::stringstream ss;
        ss << "SELECT add_deal_of_special_drug(" << _storeId << ", '"
            << _buyerName << "', '" << _buyerShenFenZheng << "', CAST(" << _buyerAge << " AS smallint), " 
            << (_buyerIsMale ? "true" : "false") << ", '"
            << _drugHuoHao << "', '" << _drugMingCheng << "', '" << _drugPiHao << "', '"
            << _drugGuiGe << "', '" << _drugJiLiang << "', '" << _drugGouMaiShuLiang << "', '" 
            << _drugShengChanChangJia << "', '" << _drugChuFangLaiYuan << "')";

        _pResult = PQexec(pConn, ss.str().c_str());

        bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        std::string valStr(PQgetvalue(_pResult,0,0));
        succ = Utils::string2Num<Poco::UInt32>(valStr, _insertDealId);
        if (!succ)
        {
            LOG_DEBUG("Failed to get deal id from the DB command result");
            PQclear(_pResult);
            return succ;
        }

        PQclear(_pResult);
        return succ;
    }

    bool DBCmd_AddDealOfSpecialDrug::_executeUpdate(PGconn *pConn)
    {
        // ----------------------------------------------------------------------
        std::stringstream ss;
        ss << "UPDATE tbl_DealOfSpecialDrug SET f_store_id=" << _storeId << ", f_time=NOW(), f_buyer_name='"  
            << _buyerName << "', f_buyer_shenfenzheng='" << _buyerShenFenZheng << "', f_buyer_age=" << _buyerAge 
            << ", f_buyer_is_male=" << (_buyerIsMale ? "true" : "false") 
            << ", f_drug_huohao='"	<< _drugHuoHao << "', f_drug_mingcheng='" << _drugMingCheng 
            << "', f_drug_pihao='" << _drugPiHao << "', f_drug_guige='" << _drugGuiGe 
            << "', f_drug_jiliang='" << _drugJiLiang << "', f_drug_goumaishuliang='" << _drugGouMaiShuLiang
            << "', f_drug_shengchanchangjia='" 
            << _drugShengChanChangJia << "', f_drug_chufanglaiyuan='" << _drugChuFangLaiYuan 
            << "' WHERE f_id=" << _updateDealId;

        _pResult = PQexec(pConn, ss.str().c_str());

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

    bool DBCmd_AddDealOfSpecialDrug::isSuccess()
    {
        return (getDealId() != 0);
    }

    Poco::UInt32 DBCmd_AddDealOfSpecialDrug::getDealId()
    {
        return (_updateDealId == 0 ? _insertDealId : _updateDealId);
    }

    void DBCmd_AddDealOfSpecialDrug::reset()
    {   
        _insertDealId = 0;
    }

} }