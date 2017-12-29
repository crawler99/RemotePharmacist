#pragma once

#include "DBCmd.h"
#include "Enums.h"
#include "Poco/Foundation.h"

namespace sserver { namespace server { 

    class DBCmd_AddDealOfSpecialDrug : public DBCmd
    {
    public:
        DBCmd_AddDealOfSpecialDrug(
            Poco::UInt32       updateDealId,
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
            const std::string &drugChuFangLaiYuan
            );
        // ------------------------------------------------------------
        bool execute(PGconn *pConn);
        void reset();
        // ------------------------------------------------------------
        bool            isSuccess();
        Poco::UInt32    getDealId();

    private:
        bool _executeInsert(PGconn *pConn);
        bool _executeUpdate(PGconn *pConn);

        // input parameters
        Poco::UInt32    _updateDealId;
        Poco::UInt32    _storeId;
        std::string     _buyerName;
        std::string     _buyerShenFenZheng;
        Poco::UInt32    _buyerAge;
        bool            _buyerIsMale;
        std::string     _drugHuoHao;
        std::string     _drugMingCheng;
        std::string     _drugPiHao;
        std::string     _drugGuiGe;
        std::string     _drugJiLiang;
        std::string     _drugGouMaiShuLiang;
        std::string     _drugShengChanChangJia;
        std::string     _drugChuFangLaiYuan;

        // execution result
        Poco::UInt32    _insertDealId;
        PGresult        *_pResult;
    };

} }