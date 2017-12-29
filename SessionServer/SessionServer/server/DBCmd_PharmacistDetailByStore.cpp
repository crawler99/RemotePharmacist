#include "DBCmd_PharmacistDetailByStore.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

    DBCmd_PharmacistDetailByStore::DBCmd_PharmacistDetailByStore(Poco::UInt32 storeId) 
        : _storeId(storeId)
    {
    }

    DBCmd_PharmacistDetailByStore::~DBCmd_PharmacistDetailByStore()
    {
        reset(); // we have to release the memory on destruction
    }

    bool DBCmd_PharmacistDetailByStore::execute(PGconn *pConn)
    {
		// ----------------------------------------------------------------------
        std::stringstream ss;
        ss << "SELECT tbl_Pharmacists.f_real_name, tbl_Pharmacists.f_certificate_type_id, tbl_Pharmacists.f_intro, tbl_Pharmacists.f_photo "
            << "FROM tbl_Users, tbl_Pharmacists, tbl_Stores, tbl_Stores_Pharmacists_Map "
            << "WHERE (tbl_Stores.f_id = tbl_Stores_Pharmacists_Map.f_store_id ) "
            << "AND (tbl_Pharmacists.f_id = tbl_Stores_Pharmacists_Map.f_pharmacist_id) "
            << "AND (tbl_Stores.f_id=" << _storeId << ")"
            << "AND (tbl_Pharmacists.f_id=tbl_Users.f_id)"
            << "AND (tbl_Users.f_is_active=true)";

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
            item._realName = PQgetvalue(_pResult,i,0);

			std::string valStr(PQgetvalue(_pResult, i, 1));
			succ = Utils::string2Num<Poco::UInt32>(valStr, item._certTypeId);
			if (!succ)
			{
				LOG_DEBUG("Failed to get certificate type id from the DB command result");
				PQclear(_pResult);
				return succ;
			}

            // store the intro content in heap
            item._introductionLen = PQgetlength(_pResult,i,2);
            if (item._introductionLen != 0)
            {
                char *pContent = PQgetvalue(_pResult,i,2);

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
            item._photoLen = PQgetlength(_pResult,i,3);
            if (item._photoLen != 0)
            {
                char *pContent = PQgetvalue(_pResult,i,3);

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
        return succ;
    }

    void DBCmd_PharmacistDetailByStore::reset()
    {
        std::vector<Item>::iterator iter = _resultItems.begin();
        while (iter != _resultItems.end())
        {
            if ((*iter)._pPhoto != NULL)
            {
                delete [] (*iter)._pPhoto;
                (*iter)._pPhoto = NULL;
            }
            ++iter;
        }
        _resultItems.clear();
    }

    const std::vector<DBCmd_PharmacistDetailByStore::Item>& DBCmd_PharmacistDetailByStore::getItems()
    {
        return _resultItems;
    }

} }