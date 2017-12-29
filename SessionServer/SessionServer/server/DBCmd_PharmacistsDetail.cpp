#include "DBCmd_PharmacistsDetail.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server {
	DBCmd_PharmacistsDetail::DBCmd_PharmacistsDetail(Poco::UInt32 userId) : _userId(userId)
	{
	}

    DBCmd_PharmacistsDetail::~DBCmd_PharmacistsDetail()
    {
        reset(); // we have to release the memory on destruction
    }

    bool DBCmd_PharmacistsDetail::execute(PGconn *pConn)
    {
		// ---------------------------------------------------------------------------------------
        std::stringstream ss;
        ss << "SELECT tbl_Users.f_id, tbl_Users.f_username, tbl_Pharmacists.f_shenfenzheng, "
            << "tbl_Pharmacists.f_real_name, tbl_Pharmacists.f_certificate_type_id, "
            << "tbl_Pharmacists_Stats.f_status, tbl_Pharmacists.f_certificate, tbl_Pharmacists.f_fingerprint, tbl_Pharmacists.f_signature, "
            << "tbl_Pharmacists.f_intro, tbl_Pharmacists.f_photo "
            << "FROM tbl_Users, tbl_Pharmacists, tbl_Pharmacists_Stats "
            << "WHERE (tbl_Users.f_id = tbl_Pharmacists.f_id ) "
            << "AND (tbl_Users.f_id = tbl_Pharmacists_Stats.f_id) "
            << "AND (tbl_Users.f_is_active=true)";

		if (_userId > 0)
		{
			ss << " AND (tbl_Users.f_id = " << _userId << ")";
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
            succ = Utils::string2Num<Poco::UInt32>(valStr, item._userId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get user id from the DB command result");
                PQclear(_pResult);
                return succ;
            }

            item._username = PQgetvalue(_pResult,i,1);
            item._shenfenzheng = PQgetvalue(_pResult,i,2);
            item._realName = PQgetvalue(_pResult,i,3);

            valStr = PQgetvalue(_pResult,i,4);
            succ = Utils::string2Num<Poco::UInt32>(valStr, item._certTypeId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get certificate type id from the DB command result");
                PQclear(_pResult);
                return succ;
            }

            valStr = PQgetvalue(_pResult,i,5);
            succ = Utils::string2Num<Poco::UInt8>(valStr, item._status);
            if (!succ)
            {
                LOG_DEBUG("Failed to get user status from the DB command result");
                PQclear(_pResult);
                return succ;
            }

            // store the certificate content in heap
            item._certLen = PQgetlength(_pResult,i,6);
            if (item._certLen != 0)
            {
                char *pContent = PQgetvalue(_pResult,i,6);

                // convert to binary
                size_t len;
                unsigned char *pTemp = PQunescapeBytea(reinterpret_cast<const unsigned char*>(pContent), &len);
                // copy the result out
                item._pCert = new unsigned char [len];
                memcpy(item._pCert, pTemp, len);
                item._certLen = len;
                PQfreemem(pTemp);
            }
            else
            {
                item._pCert = NULL;
            }

            // store the fingerprint content in heap
            item._fpLen = PQgetlength(_pResult,i,7);
            if (item._fpLen != 0)
            {
                char *pContent = PQgetvalue(_pResult,i,7);

                // convert to binary
                size_t len;
                unsigned char *pTemp = PQunescapeBytea(reinterpret_cast<const unsigned char*>(pContent), &len);
                // copy the result out
                item._pFingerprint = new unsigned char [len];
                memcpy(item._pFingerprint, pTemp, len);
                item._fpLen = len;
                PQfreemem(pTemp);
            }
            else
            {
                item._pFingerprint = NULL;
            }

            // store the signature content in heap
            item._signatureLen = PQgetlength(_pResult,i,8);
            if (item._signatureLen != 0)
            {
                char *pContent = PQgetvalue(_pResult,i,8);

                // convert to binary
                size_t len;
                unsigned char *pTemp = PQunescapeBytea(reinterpret_cast<const unsigned char*>(pContent), &len);
                // copy the result out
                item._pSignature = new unsigned char [len];
                memcpy(item._pSignature, pTemp, len);
                item._signatureLen = len;
                PQfreemem(pTemp);
            }
            else
            {
                item._pSignature = NULL;
            }

            // store the intro content in heap
            item._introductionLen = PQgetlength(_pResult,i,9);
            if (item._introductionLen != 0)
            {
                char *pContent = PQgetvalue(_pResult,i,9);

                // convert to binary
                size_t len;
                unsigned char *pTemp = PQunescapeBytea(reinterpret_cast<const unsigned char*>(pContent), &len);
                // copy the result out
                item._pIntroduction = new unsigned char [len];
                memcpy(item._pIntroduction, pTemp, len);
                item._introductionLen = len;
                PQfreemem(pTemp);
            }
            else
            {
                item._pIntroduction = NULL;
            }

            // store the photo content in heap
            item._photoLen = PQgetlength(_pResult,i,10);
            if (item._photoLen != 0)
            {
                char *pContent = PQgetvalue(_pResult,i,10);

                // convert to binary
                size_t len;
                unsigned char *pTemp = PQunescapeBytea(reinterpret_cast<const unsigned char*>(pContent), &len);
                // copy the result out
                item._pPhoto = new unsigned char [len];
                memcpy(item._pPhoto, pTemp, len);
                item._photoLen = len;
                PQfreemem(pTemp);
            }
            else
            {
                item._pPhoto = NULL;
            }

            _resultItems.push_back(item);
        }

        PQclear(_pResult);

		// ---------------------------------------------------------------------------------------
		std::vector<Item>::iterator it = _resultItems.begin();
		while (it != _resultItems.end())
		{
			ss.str("");
			ss << "SELECT tbl_Stores_Pharmacists_Map.f_store_id "
				<< "FROM tbl_Stores_Pharmacists_Map "
				<< "WHERE (tbl_Stores_Pharmacists_Map.f_pharmacist_id = " << it->_userId << ")";

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
				std::string valStr(PQgetvalue(_pResult, i, 0));
				Poco::UInt32 t;
				succ = Utils::string2Num<Poco::UInt32>(valStr, t);
				if (!succ)
				{
					LOG_DEBUG("Failed to get store id from the DB command result");
					PQclear(_pResult);
					return succ;
				}
				it->_coveredStoreIds.insert(t);
			}

			++it;
		}

		PQclear(_pResult);
        return succ;
    }

    void DBCmd_PharmacistsDetail::reset()
    {
        std::vector<DBCmd_PharmacistsDetail::Item>::iterator iter = _resultItems.begin();
        while (iter != _resultItems.end())
        {
            if ((*iter)._pCert != NULL)
            {
                delete [] (*iter)._pCert;
                (*iter)._pCert = NULL;
            }

            if ((*iter)._pFingerprint != NULL)
            {
                delete [] (*iter)._pFingerprint;
                (*iter)._pFingerprint = NULL;
            }

            if ((*iter)._pIntroduction != NULL)
            {
                delete [] (*iter)._pIntroduction;
                (*iter)._pIntroduction = NULL;
            }
            
            if ((*iter)._pPhoto != NULL)
            {
                delete [] (*iter)._pPhoto;
                (*iter)._pPhoto = NULL;
            }
            ++iter;
        }
        _resultItems.clear();
    }

    const std::vector<DBCmd_PharmacistsDetail::Item>& DBCmd_PharmacistsDetail::getItems()
    {
        return _resultItems;
    }

} }