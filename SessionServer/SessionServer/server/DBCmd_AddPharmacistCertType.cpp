#include "DBCmd_AddPharmacistCertType.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_AddPharmacistCertType::DBCmd_AddPharmacistCertType(const std::string &certTypeName) 
        : _certTypeName(certTypeName)
    {
        reset();
    }

    bool DBCmd_AddPharmacistCertType::execute(PGconn *pConn)
    {
        if (_certTypeName.empty()) return false;

        // ----------------------------------------------------------------------
        std::stringstream ss;
        ss << "INSERT INTO tbl_Pharmacist_Cert_Types (f_type_name) VALUES ('" << _certTypeName << "')"
            << " RETURNING f_id";

        _pResult = PQexec(pConn, ss.str().c_str());

        bool succ = (PGRES_TUPLES_OK == PQresultStatus(_pResult));
        if (!succ)
        {
            LOG_DEBUG(PQresultErrorMessage(_pResult));
            PQclear(_pResult);
            return succ;
        }

        std::string valStr(PQgetvalue(_pResult,0,0));
        succ = Utils::string2Num<Poco::UInt32>(valStr, _id);
        if (!succ)
        {
            LOG_DEBUG("Failed to get certificate type id from the DB command result");
            PQclear(_pResult);
            return succ;
        }

        PQclear(_pResult);
        return succ;
    }

    bool DBCmd_AddPharmacistCertType::isSuccess()
    {
        return (_id != 0);
    }

    Poco::UInt32 DBCmd_AddPharmacistCertType::getId()
    {
        return _id;
    }

    void DBCmd_AddPharmacistCertType::reset()
    {   
        _id = 0;
    }

} }