#include "Worker.h"
#include "../message/ClientMessages.h"
#include "../message/ServerMessages.h"
#include "WorkerManager.h"
#include "SimpleFileSaver.h"
#include "DBCmd_UserLogin.h"
#include "DBCmd_RefreshFingerprint.h"
#include "DBCmd_AddPharmacistCertType.h"
#include "DBCmd_ListPharmacistCertTypes.h"
#include "DBCmd_AddStore.h"
#include "DBCmd_DeleteStores.h"
#include "DBCmd_ListStores.h"
#include "DBCmd_PharmacistAdd.h"
#include "DBCmd_DeletePharmacists.h"
#include "DBCmd_PharmacistsBrief.h"
#include "DBCmd_PharmacistsDetail.h"
#include "DBCmd_PatientAdd.h"
#include "DBCmd_PharmacistDetailByStore.h"
#include "DBCmd_PharmacistIdByStore.h"
#include "DBCmd_LogConversation.h"
#include "DBCmd_SavePrescriptionLoc.h"
#include "DBCmd_EndConversation.h"
#include "DBCmd_PharmacistLogout.h"
#include "DBCmd_AddDealOfPrescriptionDrug.h"
#include "DBCmd_ListDealsOfPrescriptionDrug.h"
#include "DBCmd_AddDealOfSpecialDrug.h"
#include "DBCmd_ListDealsOfSpecialDrug.h"
#include "DBCmd_ListConsultingDetails.h"
#include "DBCmd_ListPharmacistActivity.h"
#include "SessionIdGenerator.h"
#include "../util/Utils.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Path.h"

namespace sserver { namespace server {

    void Worker::_onLoginRequest(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::LoginRequest *req = reinterpret_cast<message::LoginRequest*>(pMsg);

        // get fingerprint
        char *p = reinterpret_cast<char*>(req+1);
        message::BinaryBlock *blk = reinterpret_cast<message::BinaryBlock*>(p);
        Poco::UInt32 fpLen = blk->getContentLen();
        char *fpContent = reinterpret_cast<char*>(blk+1);

        DBCmd_UserLogin cmd(req->getUsername(), req->getPassword(), fpContent, fpLen);
        if (_pDBM->syncExec(cmd) && cmd.isSuccess())
        {
            const DBCmd_UserLogin::Item &item = cmd.getResultItem();

			Poco::UInt32 sessionId = SessionIdGenerator::getNextSessionId(item._userRole);
			std::string sessionIdStr;
			Utils::num2String<Poco::UInt32>(sessionId, sessionIdStr);
			LOG_INFO(std::string("Login succeeded: user_name=") + req->getUsername() + ", peer=" + 
				     sock.peerAddress().toString() + ", sessionId=" + sessionIdStr);

			// send back login accepted message, with session id assigned
			sserver::message::LoginAccept resp;
			resp._header.setPktLen(sizeof(resp) + 
                                   2 * sizeof(sserver::message::BinaryBlock) + 
                                   item._fpLen + item._signatureLen);

			resp._header.setSessionId(sessionId);
            resp.setUsername(req->getUsername());
			resp.setUserId(item._userId);
			resp.setUserRole(item._userRole);
			resp.setRegTime(item._regTime);
			Utils::sendBytes(sock, &resp, sizeof(resp));

            message::BinaryBlock fp;
            fp.setContentLen(item._fpLen);
            Utils::sendBytes(sock, &fp, sizeof(fp));
            Utils::sendBytes(sock, item._pFingerprint, item._fpLen);

            message::BinaryBlock signature;
            signature.setContentLen(item._signatureLen);
            Utils::sendBytes(sock, &signature, sizeof(signature));
            Utils::sendBytes(sock, item._pSignature, item._signatureLen);

			// if a pharmacist is logged in, put it into idle list
			if (item._userRole == USER_PHARMACIST)
			{
				_pWM->onPharmacistIn(sessionId, PharmacistConn(sessionId, item._userId, item._certTypeId, sock));
			}
			else if (item._userRole == USER_PATIENT)
			{
				_pWM->onPatientIn(sessionId, PatientConn(sessionId, item._userId, sock, item._storeId));
			}

			// insert the initial heartbeat
			sserver::message::ClientHeartbeat msg;
			msg._header.setSessionId(sessionId);
			msg._header.setPktLen(sizeof(msg));
			_onClientHeartbeat(sock, reinterpret_cast<char*>(&msg));
		}
        else
        {
			LOG_INFO(std::string("Login failed: user_name=") + req->getUsername() + ", peer=" + sock.peerAddress().toString());

			// send back login rejected message, without filling the session id
			sserver::message::LoginReject resp;
			resp._header.setPktLen(sizeof(resp));
			resp.setUsername(req->getUsername());
			Utils::sendBytes(sock, &resp, sizeof(resp));
        }
    }

	void Worker::_onRefreshFingerprint(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::RefreshFingerprint *req = reinterpret_cast<message::RefreshFingerprint*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		const PharmacistConn *pharmacistConn = _pWM->getPharmacistConn(sessionId);
		if (pharmacistConn != NULL)
		{
			DBCmd_RefreshFingerprint cmd(pharmacistConn->_userId);
            _pDBM->syncExec(cmd);
		}
	}

    void Worker::_onAddPharmacistCertType(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::AddPharmacistCertType *req = reinterpret_cast<message::AddPharmacistCertType*>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        DBCmd_AddPharmacistCertType cmd(req->getCertTypeName());
        if (_pDBM->syncExec(cmd) && cmd.isSuccess())
        {
            message::AddPharmacistCertTypeSucc resp;
            resp._header.setPktLen(sizeof(resp));
            resp._header.setSessionId(sessionId);
            resp.setCertTypeName(req->getCertTypeName());
            Utils::sendBytes(sock, &resp, sizeof(resp));
        }
        else
        {
            message::AddPharmacistCertTypeFail resp;
            resp._header.setPktLen(sizeof(resp));
            resp._header.setSessionId(sessionId);
            resp.setCertTypeName(req->getCertTypeName());
            Utils::sendBytes(sock, &resp, sizeof(resp));
        }
    }

    void Worker::_onListPharmacistCertTypes(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::ListPharmacistCertTypes *req = reinterpret_cast<message::ListPharmacistCertTypes*>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        DBCmd_ListPharmacistCertTypes cmd;
        if (_pDBM->syncExec(cmd))
        {
            const std::vector<DBCmd_ListPharmacistCertTypes::Item> &items = cmd.getCertTypes();

            // compose response messages
            message::PharmacistCertTypes resp;
            resp._header.setSessionId(sessionId);
            resp.setNumOfCertTypes(items.size());
            resp._header.setPktLen(sizeof(resp) + resp.getNumOfCertTypes() * sizeof(message::PharmacistCertType_Item));
            
            // send out the main block
            Utils::sendBytes(sock, &resp, sizeof(resp));

            // go items again to send out item blocks
            std::vector<DBCmd_ListPharmacistCertTypes::Item>::const_iterator iter = items.begin();
            while (iter != items.end())
            {
                message::PharmacistCertType_Item item;
                item.setId((*iter)._id);
                item.setCertTypeName((*iter)._certTypeName);
                Utils::sendBytes(sock, &item, sizeof(item));
                ++iter;
            }
        }
    }

	void Worker::_onAddStore(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::AddStore *req = reinterpret_cast<message::AddStore*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		std::set<Poco::UInt32> pharmacistIds;
		Poco::UInt32 *p = reinterpret_cast<Poco::UInt32*>(req+1);
		for (unsigned int i = 0; i < req->getNumOfPharmacists(); ++i)
		{
			Poco::UInt32 localVal = Utils::readNumericField<Poco::UInt32>(*p);
			pharmacistIds.insert(localVal);
			++p;
		}

		message::BinaryBlock *blk = reinterpret_cast<message::BinaryBlock*>(p);
		Poco::UInt32 nameLen = blk->getContentLen();
		std::string name(reinterpret_cast<char*>(blk+1), nameLen);

		blk = reinterpret_cast<message::BinaryBlock*>(pMsg + sizeof(message::AddStore) + 
			                                          req->getNumOfPharmacists() * sizeof(Poco::UInt32) +
			                                          sizeof(message::BinaryBlock) + nameLen);
		Poco::UInt32 addrLen = blk->getContentLen();
		std::string addr(reinterpret_cast<char*>(blk+1), addrLen);

		DBCmd_AddStore cmd(req->getUpdateStoreId(), name, addr, pharmacistIds);
		if (_pDBM->syncExec(cmd) && cmd.isSuccess())
		{
			message::AddStoreSucc resp;
			resp._header.setPktLen(sizeof(resp) + sizeof(message::BinaryBlock) + nameLen);
			resp._header.setSessionId(sessionId);
            resp.setStoreId(cmd.getStoreId());
            Utils::sendBytes(sock, &resp, sizeof(resp));

            message::BinaryBlock binBlk;
            binBlk.setContentLen(nameLen);
            Utils::sendBytes(sock, &binBlk, sizeof(binBlk));
            Utils::sendBytes(sock, name.c_str(), nameLen);
		}
		else
		{
			message::AddStoreFail resp;
			resp._header.setPktLen(sizeof(resp) + sizeof(message::BinaryBlock) + nameLen);
			resp._header.setSessionId(sessionId);
            Utils::sendBytes(sock, &resp, sizeof(resp));

            message::BinaryBlock binBlk;
            binBlk.setContentLen(nameLen);
            Utils::sendBytes(sock, &binBlk, sizeof(binBlk));
            Utils::sendBytes(sock, name.c_str(), nameLen);
		}
	}

	void Worker::_onDeleteStores(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::DeleteStores *req = reinterpret_cast<message::DeleteStores*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		std::set<Poco::UInt32> storeIds;
		Poco::UInt32 *p = reinterpret_cast<Poco::UInt32*>(req+1);
		for (unsigned int i = 0; i < req->getNumOfStores(); ++i)
		{
			Poco::UInt32 localVal = Utils::readNumericField<Poco::UInt32>(*p);
			storeIds.insert(localVal);
			++p;
		}

		DBCmd_DeleteStores cmd(storeIds);
		if (_pDBM->syncExec(cmd))
		{
			message::DeleteStoresSucc resp;
			resp._header.setPktLen(sizeof(resp));
			resp._header.setSessionId(sessionId);
			Utils::sendBytes(sock, &resp, sizeof(resp));
		}
		else
		{
			message::DeleteStoresFail resp;
			resp._header.setPktLen(sizeof(resp));
			resp._header.setSessionId(sessionId);
			Utils::sendBytes(sock, &resp, sizeof(resp));
		}
	}

	void Worker::_onListStores(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::ListStroes *req = reinterpret_cast<message::ListStroes*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		DBCmd_ListStores cmd;
		if (_pDBM->syncExec(cmd))
		{
            const std::map<Poco::UInt32, DBCmd_ListStores::Item> &items = cmd.getStores();

			// compose response messages
			message::Stores resp;
			resp._header.setSessionId(sessionId);
			resp.setNumOfStores(items.size());

			// go through the items to sum up length
			Poco::UInt32 totalLen = sizeof(resp);
            std::map<Poco::UInt32, DBCmd_ListStores::Item>::const_iterator iter = items.begin();
			while (iter != items.end())
			{
				totalLen += sizeof(message::Store_Item) 
                            + 2 * sizeof(message::BinaryBlock)
					        + iter->second._name.length()
							+ iter->second._addr.length();
                
                std::set<std::string>::const_iterator        
                    iiter = iter->second._patientAccts.begin();
                while (iiter != iter->second._patientAccts.end())
                {
                    totalLen += sizeof(message::BinaryBlock) + (*iiter).length();
                    ++iiter;
                }

				++iter;
			}

			// send out the main block
			resp._header.setPktLen(totalLen);
			Utils::sendBytes(sock, &resp, sizeof(resp));

			// go through the items again to send out item blocks
			iter = items.begin();
			while (iter != items.end())
			{
				message::Store_Item item;
				item.setId(iter->first);
                item.setNumOfPatientAccts(iter->second._patientAccts.size());
				Utils::sendBytes(sock, &item, sizeof(item));

                message::BinaryBlock binBlk;
                binBlk.setContentLen(iter->second._name.length());
                Utils::sendBytes(sock, &binBlk, sizeof(binBlk));
				Utils::sendBytes(sock, iter->second._name.c_str(), iter->second._name.length());

                binBlk.setContentLen(iter->second._addr.length());
                Utils::sendBytes(sock, &binBlk, sizeof(binBlk));
				Utils::sendBytes(sock, iter->second._addr.c_str(), iter->second._addr.length());

                std::set<std::string>::const_iterator 
                    iiter = iter->second._patientAccts.begin();
                while (iiter != iter->second._patientAccts.end())
                {
                    binBlk.setContentLen((*iiter).length());
                    Utils::sendBytes(sock, &binBlk, sizeof(binBlk));
                    Utils::sendBytes(sock, (*iiter).c_str(), (*iiter).length());
                    ++iiter;
                }

				++iter;
			}
		}
	}

    void Worker::_onPharmacistAdd(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::PharmacistAdd *req = reinterpret_cast<message::PharmacistAdd*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

		char *p = reinterpret_cast<char*>(req+1);

		message::BinaryBlock *blk = reinterpret_cast<message::BinaryBlock*>(p);
		Poco::UInt32 len = blk->getContentLen();
		std::string shenFenZheng(reinterpret_cast<char*>(blk+1), len);
		p += sizeof(message::BinaryBlock) + len;

		blk = reinterpret_cast<message::BinaryBlock*>(p);
		Poco::UInt32 certLen = blk->getContentLen();
		char *certContent = reinterpret_cast<char*>(blk+1);
		p += sizeof(message::BinaryBlock) + certLen;

		blk = reinterpret_cast<message::BinaryBlock*>(p);
		Poco::UInt32 fpLen = blk->getContentLen();
		char *fpContent = reinterpret_cast<char*>(blk+1);
		p += sizeof(message::BinaryBlock) + fpLen;

        blk = reinterpret_cast<message::BinaryBlock*>(p);
        Poco::UInt32 signatureLen = blk->getContentLen();
        char *signatureContent = reinterpret_cast<char*>(blk+1);
        p += sizeof(message::BinaryBlock) + signatureLen;
		
		blk = reinterpret_cast<message::BinaryBlock*>(p);
		Poco::UInt32 introLen = blk->getContentLen();
		char *introContent = reinterpret_cast<char*>(blk+1);
		p += sizeof(message::BinaryBlock) + introLen;

		blk = reinterpret_cast<message::BinaryBlock*>(p);
		Poco::UInt32 photoLen = blk->getContentLen();
		char *photoContent = reinterpret_cast<char*>(blk+1);
		p += sizeof(message::BinaryBlock) + photoLen;

		// insert to database
        DBCmd_PharmacistAdd cmd(req->getUpdatePharmacistId(), req->getUsername(), req->getPassword(), 
			                    shenFenZheng, req->getRealName(), req->getCertTypeId(),
                                certContent, certLen, fpContent, fpLen, signatureContent, signatureLen,
			                    introContent, introLen, photoContent, photoLen);
        if (_pDBM->syncExec(cmd) && cmd.isSuccess())
        {
			message::PharmacistAddSucc resp;
			resp._header.setPktLen(sizeof(resp));
			resp._header.setSessionId(sessionId);
			resp.setUsername(req->getUsername());
			Utils::sendBytes(sock, &resp, sizeof(resp));
        }
		else
		{
			message::PharmacistAddFail resp;
			resp._header.setPktLen(sizeof(resp));
			resp._header.setSessionId(sessionId);
			resp.setUsername(req->getUsername());
			Utils::sendBytes(sock, &resp, sizeof(resp));
        }
    }

    void Worker::_onDeletePharmacists(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::DeletePharmacists *req = reinterpret_cast<message::DeletePharmacists*>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        std::set<Poco::UInt32> pharmacistIds;
        Poco::UInt32 *p = reinterpret_cast<Poco::UInt32*>(req+1);
        for (unsigned int i = 0; i < req->getNumOfPharmacists(); ++i)
        {
            Poco::UInt32 localVal = Utils::readNumericField<Poco::UInt32>(*p);
            pharmacistIds.insert(localVal);
            ++p;
        }

        DBCmd_DeletePharmacists cmd(pharmacistIds);
        if (_pDBM->syncExec(cmd))
        {
            message::DeletePharmacistsSucc resp;
            resp._header.setPktLen(sizeof(resp));
            resp._header.setSessionId(sessionId);
            Utils::sendBytes(sock, &resp, sizeof(resp));
        }
        else
        {
            message::DeletePharmacistsFail resp;
            resp._header.setPktLen(sizeof(resp));
            resp._header.setSessionId(sessionId);
            Utils::sendBytes(sock, &resp, sizeof(resp));
        }
    }
    
    void Worker::_onPharmacistsBriefList(Poco::Net::StreamSocket &sock, char *pMsg)
    {
		message::PharmacistsBriefList *req = reinterpret_cast<message::PharmacistsBriefList *>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		// query to database
		DBCmd_PharmacistsBrief cmd(req->getIncludePhoto());
		if (_pDBM->syncExec(cmd))
		{
			const std::vector<DBCmd_PharmacistsBrief::Item> &items = cmd.getItems();
            
            // compose response messages
            message::PharmacistsBrief resp;
            resp._header.setSessionId(sessionId);
            resp.setNumOfPharmacists(items.size());

            // go through the items to sum up length
            Poco::UInt32 totalLen = sizeof(resp);
            std::vector<DBCmd_PharmacistsBrief::Item>::const_iterator iter = items.begin();
            while (iter != items.end())
            {
                totalLen += sizeof(message::PharmacistsBrief_Item);
                if ((*iter)._pPhoto != NULL)
                {
                    totalLen += sizeof(sserver::message::BinaryBlock) + (*iter)._photoLen;
                }
                ++iter;
            }

            // send out the main block
            resp._header.setPktLen(totalLen);
            Utils::sendBytes(sock, &resp, sizeof(resp));

            // go through the items again to send out item blocks
            iter = items.begin();
            while (iter != items.end())
            {
                message::PharmacistsBrief_Item item;
                item.setUserId((*iter)._userId);
                item.setRealName((*iter)._realName);
                item.setCertTypeId((*iter)._certTypeId);
                item.setStatus((*iter)._status);
                item.setHasPhoto((*iter)._pPhoto != NULL);
                Utils::sendBytes(sock, &item, sizeof(item));    

                if (item.getHasPhoto())
                {
                    message::BinaryBlock photo;
                    photo.setContentLen((*iter)._photoLen);
                    Utils::sendBytes(sock, &photo, sizeof(photo));
                    Utils::sendBytes(sock, (*iter)._pPhoto, (*iter)._photoLen);
                }
                ++iter;
            }
		}
    }

    void Worker::_onPharmacistsDetailList(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::PharmacistsDetailList *req = reinterpret_cast<message::PharmacistsDetailList *>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        // query to database
        DBCmd_PharmacistsDetail cmd(req->getUserId());
        if (_pDBM->syncExec(cmd))
        {
            const std::vector<DBCmd_PharmacistsDetail::Item> &items = cmd.getItems();

            // compose response messages
            message::PharmacistsDetail resp;
            resp._header.setSessionId(sessionId);
            resp.setNumOfPharmacists(items.size());

            // go through the items to sum up length
            Poco::UInt32 totalLen = sizeof(resp);
            std::vector<DBCmd_PharmacistsDetail::Item>::const_iterator iter = items.begin();
            while (iter != items.end())
            {
                totalLen += sizeof(message::PharmacistsDetail_Item);
				totalLen += (*iter)._coveredStoreIds.size() * sizeof(Poco::UInt32);
                totalLen += 6 * sizeof(sserver::message::BinaryBlock) 
                    + (*iter)._shenfenzheng.length()
                    + (*iter)._certLen
                    + (*iter)._fpLen
                    + (*iter)._signatureLen
                    + (*iter)._introductionLen
                    + (*iter)._photoLen;
                ++iter;
            }

            // send out the main block
            resp._header.setPktLen(totalLen);
            Utils::sendBytes(sock, &resp, sizeof(resp));

            // go through the items again to send out item blocks
            iter = items.begin();
            while (iter != items.end())
            {
                message::PharmacistsDetail_Item item;
                item.setUserId((*iter)._userId);
                item.setUsername((*iter)._username);
                item.setRealName((*iter)._realName);
                item.setCertTypeId((*iter)._certTypeId);
                item.setStatus((*iter)._status);
				item.setNumOfCoveredStores((*iter)._coveredStoreIds.size());
                Utils::sendBytes(sock, &item, sizeof(item));

                message::BinaryBlock shenfenzheng;
                shenfenzheng.setContentLen((*iter)._shenfenzheng.length());
                Utils::sendBytes(sock, &shenfenzheng, sizeof(shenfenzheng));
                Utils::sendBytes(sock, (*iter)._shenfenzheng.c_str(), (*iter)._shenfenzheng.length());

                message::BinaryBlock cert;
                cert.setContentLen((*iter)._certLen);
                Utils::sendBytes(sock, &cert, sizeof(cert));
                Utils::sendBytes(sock, (*iter)._pCert, (*iter)._certLen);

                message::BinaryBlock fp;
                fp.setContentLen((*iter)._fpLen);
                Utils::sendBytes(sock, &fp, sizeof(fp));
                Utils::sendBytes(sock, (*iter)._pFingerprint, (*iter)._fpLen);

                message::BinaryBlock signature;
                signature.setContentLen((*iter)._signatureLen);
                Utils::sendBytes(sock, &signature, sizeof(signature));
                Utils::sendBytes(sock, (*iter)._pSignature, (*iter)._signatureLen);

				std::set<Poco::UInt32>::const_iterator iit = (*iter)._coveredStoreIds.begin();
				while (iit != (*iter)._coveredStoreIds.end())
				{
					Poco::UInt32 netVal;
					Utils::writeNumericField<Poco::UInt32>(netVal, *iit);
					Utils::sendBytes(sock, &netVal, sizeof(netVal));
					++iit;
				}
                
                message::BinaryBlock intro;
                intro.setContentLen((*iter)._introductionLen);
                Utils::sendBytes(sock, &intro, sizeof(intro));
                Utils::sendBytes(sock, (*iter)._pIntroduction, (*iter)._introductionLen);

                message::BinaryBlock photo;
                photo.setContentLen((*iter)._photoLen);
                Utils::sendBytes(sock, &photo, sizeof(photo));
                Utils::sendBytes(sock, (*iter)._pPhoto, (*iter)._photoLen);
                
                ++iter;
            }
        }
    }

	void Worker::_onPatientAdd(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::PatientAdd *req = reinterpret_cast<message::PatientAdd*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		// insert to database
		DBCmd_PatientAdd cmd(req->getIsUpdate(), req->getUsername(), 
                             req->getPassword(), req->getStoreId());
		if (_pDBM->syncExec(cmd) && cmd.isSuccess())
		{
			message::PatientAddSucc resp;
			resp._header.setPktLen(sizeof(resp));
			resp._header.setSessionId(sessionId);
			resp.setUsername(req->getUsername());
			Utils::sendBytes(sock, &resp, sizeof(resp));
		}
		else
		{
			message::PatientAddFail resp;
			resp._header.setPktLen(sizeof(resp));
			resp._header.setSessionId(sessionId);
			resp.setUsername(req->getUsername());
			Utils::sendBytes(sock, &resp, sizeof(resp));
		}
	}

    void Worker::_onGetPharmacistsDetailOfCurStore(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::GetPharmacistsDetailOfCurrentStore *req = reinterpret_cast<message::GetPharmacistsDetailOfCurrentStore *>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        // get store id
        const PatientConn *patientConn = _pWM->getPatientConn(sessionId);
        if (patientConn != NULL)
        {
            DBCmd_PharmacistDetailByStore cmd(patientConn->_storeId);

            // query to database
            if (_pDBM->syncExec(cmd))
            {
                const std::vector<DBCmd_PharmacistDetailByStore::Item> &items = cmd.getItems();

                // compose response messages
                message::PharmacistsDetailOfCurrentStore resp;
                resp._header.setSessionId(sessionId);
                resp.setNumOfPharmacists(items.size());

                // go through the items to sum up length
                Poco::UInt32 totalLen = sizeof(resp);
                std::vector<DBCmd_PharmacistDetailByStore::Item>::const_iterator iter = items.begin();
                while (iter != items.end())
                {
                    totalLen += sizeof(message::PharmacistsDetailOfCurrentStore_Item);
                    totalLen += 2 * sizeof(sserver::message::BinaryBlock) 
                        + (*iter)._introductionLen
                        + (*iter)._photoLen;
                    ++iter;
                }

                // send out the main block
                resp._header.setPktLen(totalLen);
                Utils::sendBytes(sock, &resp, sizeof(resp));

                // go through the items again to send out item blocks
                iter = items.begin();
                while (iter != items.end())
                {
                    message::PharmacistsDetailOfCurrentStore_Item item;
                    item.setRealName((*iter)._realName);
                    item.setCertTypeId((*iter)._certTypeId);
                    Utils::sendBytes(sock, &item, sizeof(item));    

                    message::BinaryBlock intro;
                    intro.setContentLen((*iter)._introductionLen);
                    Utils::sendBytes(sock, &intro, sizeof(intro));
                    Utils::sendBytes(sock, (*iter)._pIntroduction, (*iter)._introductionLen);

                    message::BinaryBlock photo;
                    photo.setContentLen((*iter)._photoLen);
                    Utils::sendBytes(sock, &photo, sizeof(photo));
                    Utils::sendBytes(sock, (*iter)._pPhoto, (*iter)._photoLen);

                    ++iter;
                }
            }
        }
    }

    void Worker::_onPharmacistRequest(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::PharmacistRequest *req = reinterpret_cast<message::PharmacistRequest *>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

		Poco::LocalDateTime dateTime;

        // if the client request pharmacist from the global pool, do it
        // --------------------------------------------------------------------
        if (req->getUsePool())
        {
            const PatientConn *patientConn = _pWM->getPatientConn(sessionId);
            if (patientConn == NULL)
            {
                std::stringstream ss;
                ss << "Can not find the patient connection: patient sessionId=" << sessionId;
                LOG_ERROR(ss.str());
                return;
            }

            // respond to patient
            PharmacistConn pharmacist = _pWM->onRequestForPharmacist_UsePool(sock, sessionId, req->getCertTypeId());
            if (pharmacist._sessionId != 0)
            {
                message::PharmacistRequestSucc resp;
                resp._header.setPktLen(sizeof(resp));
                resp._header.setSessionId(sessionId);
                resp.setPharmacistSessionId(pharmacist._sessionId);
                Utils::sendBytes(sock, &resp, sizeof(resp));
            }
            else
            {
                message::PharmacistRequestFail resp;
                resp._header.setPktLen(sizeof(resp));
                resp._header.setSessionId(sessionId);
                Utils::sendBytes(sock, &resp, sizeof(resp));
            }
            
            // if a link is constructed, notify pharmacist and update his/her status
            if (pharmacist._sessionId != 0)
            {
                // notify pharmacist
                message::PatientConnected notif;
                notif._header.setPktLen(sizeof(notif));
                notif._header.setSessionId(pharmacist._sessionId);
                notif.setPatientSessionId(sessionId);
                Utils::sendBytes(pharmacist._sock, &notif, sizeof(notif));

                // update pharmacist status and log conversation
                std::stringstream ss;
                DBCmd_LogConversation cmd(pharmacist._userId, pharmacist._sessionId, 
                                          patientConn->_userId, patientConn->_sessionId, dateTime);
                if ((!_pDBM->syncExec(cmd)) || (!cmd.isSuccess()))
                {
                    ss << "Failed to log conversation: pharmacist sessionId=" << pharmacist._sessionId
                        << ", patient sessionId=" << patientConn->_sessionId;
                    LOG_ERROR(ss.str());
                }

                // update the conversation id
                _pWM->fillConversationId(cmd.getConversationId(), pharmacist._sessionId, patientConn->_sessionId);

                ss.str("");
                ss << "Conversation established: id=" << cmd.getConversationId() 
                    << ", pharmacist sessionId=" << pharmacist._sessionId
                    << ", patient sessionId=" << patientConn->_sessionId;
                LOG_INFO(ss.str());

				// create the fos
				ContinuousFileSaver::File f;
				f._date = Poco::DateTimeFormatter::format(dateTime, "%Y-%m-%d");
				f._time = Poco::DateTimeFormatter::format(dateTime, "%H_%M_%S");
				f._type = ContinuousFileSaver::File::CHUNK_FILE_START;

				f._generatorId = pharmacist._sessionId;
				_pWM->getVideoSaver()->addFileToSave(f);
				_pWM->getAudioSaver()->addFileToSave(f);
				f._generatorId = patientConn->_sessionId;
				_pWM->getVideoSaver()->addFileToSave(f);
				_pWM->getAudioSaver()->addFileToSave(f);
            }
            else
            {
                std::stringstream ss;
                ss << "Failed to find an idle pharmacist for service: patient sessionId=" << sessionId;
                LOG_INFO(ss.str());
            }

            return;
        }

		// otherwise, find the correct pharmacistId
        // --------------------------------------------------------------------
		const PatientConn *patientConn = _pWM->getPatientConn(sessionId);
		if (patientConn != NULL)
		{
			DBCmd_PharmacistIdByStore cmd(patientConn->_storeId, req->getCertTypeId());
			if (_pDBM->syncExec(cmd))
			{
				const std::set<Poco::UInt32> &pharmacistIds = cmd.getPharmacistIds();

				// respond to patient
                PharmacistConn pharmacist = _pWM->onRequestForPharmacist(sock, sessionId, pharmacistIds);
                if (pharmacist._sessionId != 0)
                {
                    message::PharmacistRequestSucc resp;
                    resp._header.setPktLen(sizeof(resp));
                    resp._header.setSessionId(sessionId);
                    resp.setPharmacistSessionId(pharmacist._sessionId);
                    Utils::sendBytes(sock, &resp, sizeof(resp));
                }
                else
                {
                    message::PharmacistRequestFail resp;
                    resp._header.setPktLen(sizeof(resp));
                    resp._header.setSessionId(sessionId);
                    Utils::sendBytes(sock, &resp, sizeof(resp));
                }				

				// if a link is constructed, notify pharmacist and update his/her status
				if (pharmacist._sessionId != 0)
				{
					// notify pharmacist
					message::PatientConnected notif;
					notif._header.setPktLen(sizeof(notif));
					notif._header.setSessionId(pharmacist._sessionId);
					notif.setPatientSessionId(sessionId);
					Utils::sendBytes(pharmacist._sock, &notif, sizeof(notif));

					// update pharmacist status and log conversation
					std::stringstream ss;
					DBCmd_LogConversation cmd(pharmacist._userId, pharmacist._sessionId, 
                                              patientConn->_userId, patientConn->_sessionId, dateTime);
					if ((!_pDBM->syncExec(cmd)) || (!cmd.isSuccess()))
					{
						ss << "Failed to log conversation: pharmacist sessionId=" << pharmacist._sessionId
							<< ", patient sessionId=" << patientConn->_sessionId;
						LOG_ERROR(ss.str());
					}

					// update the conversation id
					_pWM->fillConversationId(cmd.getConversationId(), pharmacist._sessionId, patientConn->_sessionId);

					ss << "Conversation established: id=" << cmd.getConversationId() 
						<< ", pharmacist sessionId=" << pharmacist._sessionId
						<< ", patient sessionId=" << patientConn->_sessionId;
					LOG_INFO(ss.str());

					// create the fos
					ContinuousFileSaver::File f;
					f._date = Poco::DateTimeFormatter::format(dateTime, "%Y-%m-%d");
					f._time = Poco::DateTimeFormatter::format(dateTime, "%H_%M_%S");
					f._type = ContinuousFileSaver::File::CHUNK_FILE_START;

					f._generatorId = pharmacist._sessionId;
					_pWM->getVideoSaver()->addFileToSave(f);
					_pWM->getAudioSaver()->addFileToSave(f);
					f._generatorId = patientConn->_sessionId;
					_pWM->getVideoSaver()->addFileToSave(f);
					_pWM->getAudioSaver()->addFileToSave(f);
				}
				else
				{
					std::stringstream ss;
					ss << "Failed to find an idle pharmacist for service: patient sessionId=" << sessionId;
					LOG_INFO(ss.str());
				}
			}
		}
    }

    void Worker::_onPrescription(Poco::Net::StreamSocket &sock, char *pMsg)
    {	
		message::Prescription *pReq = reinterpret_cast<sserver::message::Prescription*>(pMsg);
        message::BinaryBlock *binBlk = reinterpret_cast<sserver::message::BinaryBlock*>(pReq+1);
        Poco::UInt32 sessionId = pReq->_header.getSessionId();
        assert(sessionId != 0);

        std::string relativeDir = Poco::DateTimeFormatter::format(Poco::LocalDateTime(), "%Y-%m-%d");

        if (SessionIdGenerator::checkRole(sessionId, USER_PHARMACIST))
        {
            ConversationEnd_Patient *targetPatient = _pWM->getTargetPatient(sessionId);
            if (targetPatient != NULL)
            {
                DBCmd_SavePrescriptionLoc cmd(targetPatient->_conversationId, MSG_DIRECTION_PHARMACIST_TO_PATIENT, relativeDir);
                if ((!_pDBM->syncExec(cmd)) || (!cmd.isSuccess()))
                {
                    std::stringstream ss;
                    ss << "Failed to save prescription: pharmacist sessionId=" << sessionId;
                    LOG_ERROR(ss.str());
                }

                // transfer directly, without changing the sessionId field
                Utils::sendBytes(targetPatient->_conn._sock, pMsg, pReq->_header.getPktLen());

                // save to file
				std::stringstream ss;
				ss << relativeDir << Poco::Path::separator() << cmd.getStoreId();
				SimpleFileSaver::File f;
				f._relativeDir = ss.str();

				ss.str("");
				ss << cmd.getPrescriptionId();
				f._name = ss.str();

				Poco::UInt32 len = binBlk->getContentLen();
				char *pData = new char[len];
				memcpy(pData, binBlk+1, len);
				f._pData = pData;
				f._len = len;
				_pWM->getPrescriptionSaver()->addFileToSave(f);
            }
        }
        else if (SessionIdGenerator::checkRole(sessionId, USER_PATIENT))
        {
            ConversationEnd_Pharmacist *targetPharmacist = _pWM->getTargetPharmacist(sessionId);
            if (targetPharmacist != NULL)
            {
                DBCmd_SavePrescriptionLoc cmd(targetPharmacist->_conversationId, MSG_DIRECTION_PATIENT_TO_PHARMACIST, relativeDir);
                if ((!_pDBM->syncExec(cmd)) || (!cmd.isSuccess()))
                {
                    std::stringstream ss;
                    ss << "Failed to save prescription: patient sessionId=" << sessionId;
                    LOG_ERROR(ss.str());
                }

                // transfer directly, without changing the sessionId field
                Utils::sendBytes(targetPharmacist->_conn._sock, pMsg, pReq->_header.getPktLen());

                // save to file
				std::stringstream ss;
				ss << relativeDir << Poco::Path::separator() << cmd.getStoreId();
				SimpleFileSaver::File f;
				f._relativeDir = ss.str();

				ss.str("");
				ss << cmd.getPrescriptionId();
				f._name = ss.str();

				Poco::UInt32 len = binBlk->getContentLen();
				char *pData = new char[len];
				memcpy(pData, binBlk+1, len);
				f._pData = pData;
				f._len = len;
				_pWM->getPrescriptionSaver()->addFileToSave(f);
            }
        }
    }

    void Worker::_onVideo(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::Video *pReq = reinterpret_cast<sserver::message::Video*>(pMsg);
        message::BinaryBlock *binBlk = reinterpret_cast<sserver::message::BinaryBlock*>(pReq+1);
        Poco::UInt32 sessionId = pReq->_header.getSessionId();
        assert(sessionId != 0);

        if (SessionIdGenerator::checkRole(sessionId, USER_PHARMACIST))
        {
			Poco::Net::StreamSocket *targetSock = _pWM->getTargetPatientVideoSock(sessionId, sock);
			if (targetSock != NULL)
			{
                // discard empty packet directly
                if (binBlk->getContentLen() == 0) return;

				// transfer directly, without changing the sessionId field
				Utils::sendBytes(*targetSock, pMsg, pReq->_header.getPktLen());
			}
		}
		else if (SessionIdGenerator::checkRole(sessionId, USER_PATIENT))
		{
			Poco::Net::StreamSocket *targetSock = _pWM->getTargetPharmacistVideoSock(sessionId, sock);
			if (targetSock != NULL)
			{
                // discard empty packet directly
                if (binBlk->getContentLen() == 0) return;

				// transfer directly, without changing the sessionId field
				Utils::sendBytes(*targetSock, pMsg, pReq->_header.getPktLen());
			}
		}

		// save to file
		ContinuousFileSaver::File f;
		f._generatorId = sessionId;

		Poco::UInt32 len = binBlk->getContentLen();
		char *pData = new char[len];
		memcpy(pData, binBlk+1, len);
		f._pData = pData;
		f._len = len;
		_pWM->getVideoSaver()->addFileToSave(f);
    }

    void Worker::_onAudio(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::Audio *pReq = reinterpret_cast<sserver::message::Audio*>(pMsg);
        message::BinaryBlock *binBlk = reinterpret_cast<sserver::message::BinaryBlock*>(pReq+1);
        Poco::UInt32 sessionId = pReq->_header.getSessionId();
        assert(sessionId != 0);

        if (SessionIdGenerator::checkRole(sessionId, USER_PHARMACIST))
        {
            Poco::Net::StreamSocket *targetSock = _pWM->getTargetPatientAudioSock(sessionId, sock);
            if (targetSock != NULL)
            {
                // discard empty packet directly
                if (binBlk->getContentLen() == 0) return;

                // transfer directly, without changing the sessionId field
                Utils::sendBytes(*targetSock, pMsg, pReq->_header.getPktLen());
            }
        }
        else if (SessionIdGenerator::checkRole(sessionId, USER_PATIENT))
        {
            Poco::Net::StreamSocket *targetSock = _pWM->getTargetPharmacistAudioSock(sessionId, sock);
            if (targetSock != NULL)
            {
                // discard empty packet directly
                if (binBlk->getContentLen() == 0) return;

                // transfer directly, without changing the sessionId field
                Utils::sendBytes(*targetSock, pMsg, pReq->_header.getPktLen());
            }
        }

		// save to file
		ContinuousFileSaver::File f;
		f._generatorId = sessionId;

		Poco::UInt32 len = binBlk->getContentLen();
		char *pData = new char[len];
		memcpy(pData, binBlk+1, len);
		f._pData = pData;
		f._len = len;
		_pWM->getAudioSaver()->addFileToSave(f);
    }

	void Worker::_onStopConversationRequest(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::StopConversation *req = reinterpret_cast<message::StopConversation*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		if (SessionIdGenerator::checkRole(sessionId, USER_PATIENT))
		{
            PharmacistConn targetPharmacist;
			if (_pWM->onPatientQuitConversation(sessionId, targetPharmacist))
			{
                // update pharmacist status
                DBCmd_EndConversation cmd(targetPharmacist._userId);
                if (!_pDBM->syncExec(cmd))
                {
                    std::stringstream ss;
                    ss << "Failed to log pharmacist status: pharmacist sessionId=" << targetPharmacist._sessionId;
                    LOG_INFO(ss.str());
                }
			
				// notify the pharmacist
                message::PatientQuitService notif;
                notif._header.setPktLen(sizeof(notif));
                notif._header.setSessionId(targetPharmacist._sessionId);
                notif.setPatientSessionId(sessionId);
                Utils::sendBytes(targetPharmacist._sock, &notif, sizeof(notif));
			}
		}
        else if (SessionIdGenerator::checkRole(sessionId, USER_PHARMACIST))
        {
            PatientConn targetPatient;
            if (_pWM->onPharmacistQuitConversation(sessionId, targetPatient))
            {
                // update pharmacist status
                const PharmacistConn *pharmacistConn = _pWM->getPharmacistConn(sessionId);
                if (pharmacistConn != NULL)
                {
                    DBCmd_EndConversation cmd(pharmacistConn->_userId);
                    if (!_pDBM->syncExec(cmd))
                    {
                        std::stringstream ss;
                        ss << "Failed to log pharmacist status: pharmacist sessionId=" << sessionId;
                        LOG_INFO(ss.str());
                    }
                }
                
                // notify the patient
                message::PharmacistQuitService notif;
                notif._header.setPktLen(sizeof(notif));
                notif._header.setSessionId(targetPatient._sessionId);
                notif.setPharmacistSessionId(sessionId);
                Utils::sendBytes(targetPatient._sock, &notif, sizeof(notif));
            }
        }
	}

	void Worker::_onClientHeartbeat(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::ClientHeartbeat *req = reinterpret_cast<message::ClientHeartbeat*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		ClientHBExaminee examinee;
		examinee._sock = sock;
		examinee._worker = this;
		_pWM->onClientHeartbeat(sessionId, examinee);
	}

	void Worker::onLogoutRequest(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::LogoutRequest *req = reinterpret_cast<message::LogoutRequest*>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        // firstly, trigger a "stop conversation" event
        message::StopConversation sc;
        sc._header.setPktLen(sizeof(sc));
        sc._header.setSessionId(sessionId);
        _onStopConversationRequest(sock, reinterpret_cast<char*>(&sc));

        // if it's a pharmacist who requests to log out, update the status
        if (SessionIdGenerator::checkRole(sessionId, USER_PHARMACIST))
        {
            const PharmacistConn *pharmacistConn = _pWM->getPharmacistConn(sessionId);
            if (pharmacistConn != NULL)
            {
                DBCmd_PharmacistLogout cmd(pharmacistConn->_userId);
                if (!_pDBM->syncExec(cmd))
                {
                    std::stringstream ss;
                    ss << "Failed to log pharmacist status: pharmacist sessionId=" << sessionId;
                    LOG_INFO(ss.str());
                }
            }
			_pWM->onPharmacistOut(sessionId);
        }
		else if (SessionIdGenerator::checkRole(sessionId, USER_PATIENT))
		{
			_pWM->onPatientOut(sessionId);
		}
    }

    void Worker::_onShutDownRequest(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        exit(0);
    }

	void Worker::_onFakeLogout(Poco::Net::StreamSocket &sock, Poco::UInt32 sessionId)
	{
		assert(sessionId != 0);

		// firstly, trigger a "stop conversation" event
		message::StopConversation sc;
		sc._header.setPktLen(sizeof(sc));
		sc._header.setSessionId(sessionId);
		_onStopConversationRequest(sock, reinterpret_cast<char*>(&sc));

		// if it's a pharmacist who requests to log out, update the status
		if (SessionIdGenerator::checkRole(sessionId, USER_PHARMACIST))
		{
			const PharmacistConn *pharmacistConn = _pWM->getPharmacistConn(sessionId);
			if (pharmacistConn != NULL)
			{
				DBCmd_PharmacistLogout cmd(pharmacistConn->_userId);
				if (!_pDBM->syncExec(cmd))
				{
					std::stringstream ss;
					ss << "Failed to log pharmacist status: pharmacist sessionId=" << sessionId;
					LOG_INFO(ss.str());
				}
			}
			_pWM->onPharmacistOut(sessionId);
		}
		else if (SessionIdGenerator::checkRole(sessionId, USER_PATIENT))
		{
			_pWM->onPatientOut(sessionId);
		}
	}

	void Worker::_onAddDealOfPrescriptionDrug(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::AddDealOfPrescriptionDrug *req = reinterpret_cast<message::AddDealOfPrescriptionDrug*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

        const PatientConn *patientConn = _pWM->getPatientConn(sessionId);
        if (patientConn != NULL)
        {
            char *p = reinterpret_cast<char*>(req+1);

            message::BinaryBlock *blk = reinterpret_cast<message::BinaryBlock*>(p);
            Poco::UInt32 len = blk->getContentLen();
            std::string buyerName(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugHuoHao(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugMingCheng(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugPiHao(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugGuiGe(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugJiLiang(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugShengChanChangJia(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugChuFangLaiYuan(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            DBCmd_AddDealOfPrescriptionDrug cmd(req->getUpdateDealId(), patientConn->_storeId, 
                                                buyerName, req->getBuyerAge(), req->getBuyerIsMale(), 
                                                drugHuoHao, drugMingCheng, drugPiHao,
                                                drugGuiGe, drugJiLiang, drugShengChanChangJia, drugChuFangLaiYuan);
            if (_pDBM->syncExec(cmd) && cmd.isSuccess())
            {
                message::AddDealOfPrescriptionDrugSucc resp;
                resp._header.setPktLen(sizeof(resp));
                resp._header.setSessionId(sessionId);
                Utils::sendBytes(sock, &resp, sizeof(resp));
                return;
            }
        }

        message::AddDealOfPrescriptionDrugFail resp;
        resp._header.setPktLen(sizeof(resp));
        resp._header.setSessionId(sessionId);
        Utils::sendBytes(sock, &resp, sizeof(resp));
	}

    void Worker::_onListDealsOfPrescriptionDrug(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::ListDealsOfPrescriptionDrug *req = reinterpret_cast<message::ListDealsOfPrescriptionDrug*>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        DBCmd_ListDealsOfPrescriptionDrug cmd(req->getStoreId(), req->getStartTime(), req->getEndTime());
        if (_pDBM->syncExec(cmd))
        {
            const std::vector<DBCmd_ListDealsOfPrescriptionDrug::Item> &items = cmd.getItems();

            // compose response messages
            message::DealsOfPrescriptionDrugDetail resp;
            resp._header.setSessionId(sessionId);
            resp.setNumOfDeals(items.size());

            // go through the items to sum up length
            Poco::UInt32 totalLen = sizeof(resp);
            std::vector<DBCmd_ListDealsOfPrescriptionDrug::Item>::const_iterator iter = items.begin();
            while (iter != items.end())
            {
                totalLen += sizeof(message::DealsOfPrescriptionDrugDetail_Item);
                totalLen += 8 * sizeof(sserver::message::BinaryBlock) 
                    + (*iter)._buyerName.length()
                    + (*iter)._drugHuoHao.length()
                    + (*iter)._drugMingCheng.length()
                    + (*iter)._drugPiHao.length()
                    + (*iter)._drugGuiGe.length()
                    + (*iter)._drugJiLiang.length()
                    + (*iter)._drugShengChanChangJia.length()
                    + (*iter)._drugChuFangLaiYuan.length();
                ++iter;
            }

            // send out the main block
            resp._header.setPktLen(totalLen);
            Utils::sendBytes(sock, &resp, sizeof(resp));

            // go through the items again to send out item blocks
            iter = items.begin();
            while (iter != items.end())
            {
                message::DealsOfPrescriptionDrugDetail_Item item;
                item.setDealId((*iter)._dealId);
                item.setBuyerAge((*iter)._buyerAge);
                item.setBuyerIsMale((*iter)._buyerIsMale);
                item.setTime((*iter)._time);
                Utils::sendBytes(sock, &item, sizeof(item));

                message::BinaryBlock buyerName;
                buyerName.setContentLen((*iter)._buyerName.length());
                Utils::sendBytes(sock, &buyerName, sizeof(buyerName));
                Utils::sendBytes(sock, (*iter)._buyerName.c_str(), (*iter)._buyerName.length());

                message::BinaryBlock drugHuoHao;
                drugHuoHao.setContentLen((*iter)._drugHuoHao.length());
                Utils::sendBytes(sock, &drugHuoHao, sizeof(drugHuoHao));
                Utils::sendBytes(sock, (*iter)._drugHuoHao.c_str(), (*iter)._drugHuoHao.length());

                message::BinaryBlock drugMingCheng;
                drugMingCheng.setContentLen((*iter)._drugMingCheng.length());
                Utils::sendBytes(sock, &drugMingCheng, sizeof(drugMingCheng));
                Utils::sendBytes(sock, (*iter)._drugMingCheng.c_str(), (*iter)._drugMingCheng.length());
                
                message::BinaryBlock drugPiHao;
                drugPiHao.setContentLen((*iter)._drugPiHao.length());
                Utils::sendBytes(sock, &drugPiHao, sizeof(drugPiHao));
                Utils::sendBytes(sock, (*iter)._drugPiHao.c_str(), (*iter)._drugPiHao.length());

                message::BinaryBlock drugGuiGe;
                drugGuiGe.setContentLen((*iter)._drugGuiGe.length());
                Utils::sendBytes(sock, &drugGuiGe, sizeof(drugGuiGe));
                Utils::sendBytes(sock, (*iter)._drugGuiGe.c_str(), (*iter)._drugGuiGe.length());

                message::BinaryBlock drugJiLiang;
                drugJiLiang.setContentLen((*iter)._drugJiLiang.length());
                Utils::sendBytes(sock, &drugJiLiang, sizeof(drugJiLiang));
                Utils::sendBytes(sock, (*iter)._drugJiLiang.c_str(), (*iter)._drugJiLiang.length());

                message::BinaryBlock drugShengChanChangJia;
                drugShengChanChangJia.setContentLen((*iter)._drugShengChanChangJia.length());
                Utils::sendBytes(sock, &drugShengChanChangJia, sizeof(drugShengChanChangJia));
                Utils::sendBytes(sock, (*iter)._drugShengChanChangJia.c_str(), (*iter)._drugShengChanChangJia.length());

                message::BinaryBlock drugChuFangLaiYuan;
                drugChuFangLaiYuan.setContentLen((*iter)._drugChuFangLaiYuan.length());
                Utils::sendBytes(sock, &drugChuFangLaiYuan, sizeof(drugChuFangLaiYuan));
                Utils::sendBytes(sock, (*iter)._drugChuFangLaiYuan.c_str(), (*iter)._drugChuFangLaiYuan.length());

                ++iter;
            }
        }
    }

    void Worker::_onAddDealOfSpecialDrug(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::AddDealOfSpecialDrug *req = reinterpret_cast<message::AddDealOfSpecialDrug*>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        const PatientConn *patientConn = _pWM->getPatientConn(sessionId);
        if (patientConn != NULL)
        {
            char *p = reinterpret_cast<char*>(req+1);

            message::BinaryBlock *blk = reinterpret_cast<message::BinaryBlock*>(p);
            Poco::UInt32 len = blk->getContentLen();
            std::string buyerName(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string buyerShenFenZheng(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugHuoHao(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugMingCheng(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugPiHao(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugGuiGe(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugJiLiang(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugGouMaiJiLiang(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugShengChanChangJia(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            blk = reinterpret_cast<message::BinaryBlock*>(p);
            len = blk->getContentLen();
            std::string drugChuFangLaiYuan(reinterpret_cast<char*>(blk+1), len);
            p += sizeof(message::BinaryBlock) + len;

            DBCmd_AddDealOfSpecialDrug cmd(req->getUpdateDealId(), patientConn->_storeId, 
                buyerName, buyerShenFenZheng, req->getBuyerAge(), req->getBuyerIsMale(), 
                drugHuoHao, drugMingCheng, drugPiHao, drugGuiGe, drugJiLiang, 
                drugGouMaiJiLiang, drugShengChanChangJia, drugChuFangLaiYuan);
            if (_pDBM->syncExec(cmd) && cmd.isSuccess())
            {
                message::AddDealOfSpecialDrugSucc resp;
                resp._header.setPktLen(sizeof(resp));
                resp._header.setSessionId(sessionId);
                Utils::sendBytes(sock, &resp, sizeof(resp));
                return;
            }
        }

        message::AddDealOfSpecialDrugFail resp;
        resp._header.setPktLen(sizeof(resp));
        resp._header.setSessionId(sessionId);
        Utils::sendBytes(sock, &resp, sizeof(resp));
    }

    void Worker::_onListDealsOfSpecialDrug(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::ListDealsOfSpecialDrug *req = reinterpret_cast<message::ListDealsOfSpecialDrug*>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        DBCmd_ListDealsOfSpecialDrug cmd(req->getStoreId(), req->getStartTime(), req->getEndTime());
        if (_pDBM->syncExec(cmd))
        {
            const std::vector<DBCmd_ListDealsOfSpecialDrug::Item> &items = cmd.getItems();

            // compose response messages
            message::DealsOfSpecialDrugDetail resp;
            resp._header.setSessionId(sessionId);
            resp.setNumOfDeals(items.size());

            // go through the items to sum up length
            Poco::UInt32 totalLen = sizeof(resp);
            std::vector<DBCmd_ListDealsOfSpecialDrug::Item>::const_iterator iter = items.begin();
            while (iter != items.end())
            {
                totalLen += sizeof(message::DealsOfSpecialDrugDetail_Item);
                totalLen += 10 * sizeof(sserver::message::BinaryBlock) 
                    + (*iter)._buyerName.length()
                    + (*iter)._buyerShenFenZheng.length()
                    + (*iter)._drugHuoHao.length()
                    + (*iter)._drugMingCheng.length()
                    + (*iter)._drugPiHao.length()
                    + (*iter)._drugGuiGe.length()
                    + (*iter)._drugJiLiang.length()
                    + (*iter)._drugGouMaiShuLiang.length()
                    + (*iter)._drugShengChanChangJia.length()
                    + (*iter)._drugChuFangLaiYuan.length();
                ++iter;
            }

            // send out the main block
            resp._header.setPktLen(totalLen);
            Utils::sendBytes(sock, &resp, sizeof(resp));

            // go through the items again to send out item blocks
            iter = items.begin();
            while (iter != items.end())
            {
                message::DealsOfSpecialDrugDetail_Item item;
                item.setDealId((*iter)._dealId);
                item.setBuyerAge((*iter)._buyerAge);
                item.setBuyerIsMale((*iter)._buyerIsMale);
                item.setTime((*iter)._time);
                Utils::sendBytes(sock, &item, sizeof(item));

                message::BinaryBlock buyerName;
                buyerName.setContentLen((*iter)._buyerName.length());
                Utils::sendBytes(sock, &buyerName, sizeof(buyerName));
                Utils::sendBytes(sock, (*iter)._buyerName.c_str(), (*iter)._buyerName.length());

                message::BinaryBlock buyerShenFenZheng;
                buyerShenFenZheng.setContentLen((*iter)._buyerShenFenZheng.length());
                Utils::sendBytes(sock, &buyerShenFenZheng, sizeof(buyerShenFenZheng));
                Utils::sendBytes(sock, (*iter)._buyerShenFenZheng.c_str(), (*iter)._buyerShenFenZheng.length());

                message::BinaryBlock drugHuoHao;
                drugHuoHao.setContentLen((*iter)._drugHuoHao.length());
                Utils::sendBytes(sock, &drugHuoHao, sizeof(drugHuoHao));
                Utils::sendBytes(sock, (*iter)._drugHuoHao.c_str(), (*iter)._drugHuoHao.length());

                message::BinaryBlock drugMingCheng;
                drugMingCheng.setContentLen((*iter)._drugMingCheng.length());
                Utils::sendBytes(sock, &drugMingCheng, sizeof(drugMingCheng));
                Utils::sendBytes(sock, (*iter)._drugMingCheng.c_str(), (*iter)._drugMingCheng.length());

                message::BinaryBlock drugPiHao;
                drugPiHao.setContentLen((*iter)._drugPiHao.length());
                Utils::sendBytes(sock, &drugPiHao, sizeof(drugPiHao));
                Utils::sendBytes(sock, (*iter)._drugPiHao.c_str(), (*iter)._drugPiHao.length());

                message::BinaryBlock drugGuiGe;
                drugGuiGe.setContentLen((*iter)._drugGuiGe.length());
                Utils::sendBytes(sock, &drugGuiGe, sizeof(drugGuiGe));
                Utils::sendBytes(sock, (*iter)._drugGuiGe.c_str(), (*iter)._drugGuiGe.length());

                message::BinaryBlock drugJiLiang;
                drugJiLiang.setContentLen((*iter)._drugJiLiang.length());
                Utils::sendBytes(sock, &drugJiLiang, sizeof(drugJiLiang));
                Utils::sendBytes(sock, (*iter)._drugJiLiang.c_str(), (*iter)._drugJiLiang.length());

                message::BinaryBlock drugGouMaiShuLiang;
                drugGouMaiShuLiang.setContentLen((*iter)._drugGouMaiShuLiang.length());
                Utils::sendBytes(sock, &drugGouMaiShuLiang, sizeof(drugGouMaiShuLiang));
                Utils::sendBytes(sock, (*iter)._drugGouMaiShuLiang.c_str(), (*iter)._drugGouMaiShuLiang.length());

                message::BinaryBlock drugShengChanChangJia;
                drugShengChanChangJia.setContentLen((*iter)._drugShengChanChangJia.length());
                Utils::sendBytes(sock, &drugShengChanChangJia, sizeof(drugShengChanChangJia));
                Utils::sendBytes(sock, (*iter)._drugShengChanChangJia.c_str(), (*iter)._drugShengChanChangJia.length());

                message::BinaryBlock drugChuFangLaiYuan;
                drugChuFangLaiYuan.setContentLen((*iter)._drugChuFangLaiYuan.length());
                Utils::sendBytes(sock, &drugChuFangLaiYuan, sizeof(drugChuFangLaiYuan));
                Utils::sendBytes(sock, (*iter)._drugChuFangLaiYuan.c_str(), (*iter)._drugChuFangLaiYuan.length());

                ++iter;
            }
        }
    }

    void Worker::_onListConsultingDetails(Poco::Net::StreamSocket &sock, char *pMsg)
    {
        message::ListConsultingDetails *req = reinterpret_cast<message::ListConsultingDetails*>(pMsg);
        Poco::UInt32 sessionId = req->_header.getSessionId();
        assert(sessionId != 0);

        DBCmd_ListConsultingDetails cmd(req->getStoreId(), req->getStartTime(), req->getEndTime());
        if (_pDBM->syncExec(cmd))
        {
			const std::map<Poco::UInt32, DBCmd_ListConsultingDetails::Item> &items = cmd.getItems();

			// compose response messages
			message::ConsultingDetails resp;
			resp._header.setSessionId(sessionId);
			resp.setNumOfItems(items.size());

			// go through the items to sum up length
			Poco::UInt32 totalLen = sizeof(resp);
			std::map<Poco::UInt32, DBCmd_ListConsultingDetails::Item>::const_iterator iter = items.begin();
			while (iter != items.end())
			{
				totalLen += sizeof(message::ConsultingDetails_Item);
				totalLen += (4 + iter->second._prescriptionLocs.size()) * sizeof(sserver::message::BinaryBlock) 
					         + iter->second._pharmacistVideoLoc.length()
                             + iter->second._patientVideoLoc.length()
					         + iter->second._pharmacistAudioLoc.length()
                             + iter->second._patientAudioLoc.length();
				
				std::vector<std::string>::const_iterator iiter = iter->second._prescriptionLocs.begin();
				while (iiter != iter->second._prescriptionLocs.end())
				{
					totalLen += (*iiter).length();
					++iiter;
				}

				++iter;
			}

			// send out the main block
			resp._header.setPktLen(totalLen);
			Utils::sendBytes(sock, &resp, sizeof(resp));

			// go through the items again to send out item blocks
			iter = items.begin();
			while (iter != items.end())
			{
				message::ConsultingDetails_Item item;
				item.setPharmacistId(iter->second._pharmacistId);
				item.setPatientId(iter->second._patientId);
				item.setStartTime(iter->second._startTime);
				item.setEndTime(iter->second._stopTime);
                item.setNumOfPrescriptions(iter->second._prescriptionLocs.size());

				Utils::sendBytes(sock, &item, sizeof(item));

				message::BinaryBlock videoLoc1;
				videoLoc1.setContentLen(iter->second._pharmacistVideoLoc.length());
				Utils::sendBytes(sock, &videoLoc1, sizeof(videoLoc1));
				Utils::sendBytes(sock, iter->second._pharmacistVideoLoc.c_str(), iter->second._pharmacistVideoLoc.length());

                message::BinaryBlock videoLoc2;
                videoLoc2.setContentLen(iter->second._patientVideoLoc.length());
                Utils::sendBytes(sock, &videoLoc2, sizeof(videoLoc2));
                Utils::sendBytes(sock, iter->second._patientVideoLoc.c_str(), iter->second._patientVideoLoc.length());

				message::BinaryBlock audioLoc1;
				audioLoc1.setContentLen(iter->second._pharmacistAudioLoc.length());
				Utils::sendBytes(sock, &audioLoc1, sizeof(audioLoc1));
				Utils::sendBytes(sock, iter->second._pharmacistAudioLoc.c_str(), iter->second._pharmacistAudioLoc.length());

                message::BinaryBlock audioLoc2;
                audioLoc2.setContentLen(iter->second._patientAudioLoc.length());
                Utils::sendBytes(sock, &audioLoc2, sizeof(audioLoc2));
                Utils::sendBytes(sock, iter->second._patientAudioLoc.c_str(), iter->second._patientAudioLoc.length());

				std::vector<std::string>::const_iterator iiter = iter->second._prescriptionLocs.begin();
				while (iiter != iter->second._prescriptionLocs.end())
				{
					message::BinaryBlock loc;
					loc.setContentLen((*iiter).length());
					Utils::sendBytes(sock, &loc, sizeof(loc));
					Utils::sendBytes(sock, (*iiter).c_str(), (*iiter).length());
					++iiter;
				}

				++iter;
			}
        }
    }

	void Worker::_onListPharmacistActivity(Poco::Net::StreamSocket &sock, char *pMsg)
	{
		message::ListPharmacistActivity *req = reinterpret_cast<message::ListPharmacistActivity*>(pMsg);
		Poco::UInt32 sessionId = req->_header.getSessionId();
		assert(sessionId != 0);

		DBCmd_ListPharmacistActivity cmd(req->getPharmacistId(), req->getStartTime(), req->getEndTime());
		if (_pDBM->syncExec(cmd))
		{
			const std::vector<std::string> &items = cmd.getItems();

			// compose response messages
			message::PharmacistActivity resp;
			resp._header.setSessionId(sessionId);
			resp.setPharmacistId(req->getPharmacistId());
			resp.setNumOfItems(items.size());

			Poco::UInt32 totalLen = sizeof(resp);
			std::vector<std::string>::const_iterator iter = items.begin();
			while (iter != items.end())
			{
				totalLen += sizeof(sserver::message::BinaryBlock) + iter->length();
				++iter;
			}

			// send out the main block
			resp._header.setPktLen(totalLen);
			Utils::sendBytes(sock, &resp, sizeof(resp));

			// go through the items again to send out item blocks
			iter = items.begin();
			while (iter != items.end())
			{
				message::BinaryBlock blk;
				blk.setContentLen(iter->length());
				Utils::sendBytes(sock, &blk, sizeof(blk));
				Utils::sendBytes(sock, iter->c_str(), iter->length());
				++iter;
			}
		}
	}

} }