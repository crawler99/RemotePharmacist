#include "DBCmd_PharmacistsBrief.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include <sstream>

namespace sserver { namespace server { 

	DBCmd_PharmacistsBrief::DBCmd_PharmacistsBrief(bool includePhoto) 
		: _includePhoto(includePhoto)
	{
	}

	DBCmd_PharmacistsBrief::~DBCmd_PharmacistsBrief()
	{
		// we have to release the memory on destruction
		reset();
	}

	bool DBCmd_PharmacistsBrief::execute(PGconn *pConn)
	{
		// ----------------------------------------------------------------------
		std::stringstream ss;
		ss << "SELECT tbl_Users.f_id, tbl_Pharmacists.f_real_name, tbl_Pharmacists.f_certificate_type_id, "
			<< "tbl_Pharmacists_Stats.f_status"
			<< (_includePhoto ? ", tbl_Pharmacists.f_photo " : " ")
			<< "FROM tbl_Users, tbl_Pharmacists, tbl_Pharmacists_Stats "
		    << "WHERE (tbl_Users.f_id=tbl_Pharmacists.f_id ) "
            << "AND (tbl_Users.f_id=tbl_Pharmacists_Stats.f_id) "
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
			std::string valStr(PQgetvalue(_pResult, i, 0));
			succ = Utils::string2Num<Poco::UInt32>(valStr, item._userId);
			if (!succ)
			{
				LOG_DEBUG("Failed to get user id from the DB command result");
				PQclear(_pResult);
				return succ;
			}

			item._realName = PQgetvalue(_pResult,i,1);

            valStr = PQgetvalue(_pResult,i,2);
            succ = Utils::string2Num<Poco::UInt32>(valStr, item._certTypeId);
            if (!succ)
            {
                LOG_DEBUG("Failed to get certificate type id from the DB command result");
                PQclear(_pResult);
                return succ;
            }

			valStr = PQgetvalue(_pResult,i,3);
			succ = Utils::string2Num<Poco::UInt8>(valStr, item._status);
			if (!succ)
			{
				LOG_DEBUG("Failed to get user status from the DB command result");
				PQclear(_pResult);
				return succ;
			}
			
			// store the photo content in heap
			item._photoLen = (_includePhoto ? PQgetlength(_pResult,i,4) : 0);
			if (item._photoLen != 0)
			{
				char *pContent = PQgetvalue(_pResult,i,4);
				
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

	void DBCmd_PharmacistsBrief::reset()
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

	const std::vector<DBCmd_PharmacistsBrief::Item>& DBCmd_PharmacistsBrief::getItems()
	{
		return _resultItems;
	}

} }