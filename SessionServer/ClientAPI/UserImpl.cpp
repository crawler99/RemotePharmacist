#include "UserImpl.h"
#include "UserCallback.h"
#include "Enums.h"
#include "../SessionServer/message/Enums.h"
#include "../SessionServer/message/ClientMessages.h"
#include "../SessionServer/message/ServerMessages.h"
#include "../SessionServer/util/Utils.h"
#include "../SessionServer/logger/Logger.h"
#include "Poco/net/NetException.h"
#include <cassert>

namespace client {

    UserImpl::UserImpl(const std::string &username,
                       const std::string &password,
                       UserCallback *userCallback)
        : _username(username)
        , _password(password)
        , _userCallback(userCallback)
        , _sessionId(0)
        , _sock(new Poco::Net::StreamSocket())
		, _videoSock(new Poco::Net::StreamSocket())
		, _audioSock(new Poco::Net::StreamSocket())
    {
        _isReceiving = false;
        _syncEvt.reset();
    }

    bool UserImpl::connect(const std::string &serverIP, unsigned int serverPort)
    {
		_serverIP = serverIP;
		_serverPort = serverPort;
		
		try
        {
            _sock->connect(Poco::Net::SocketAddress(_serverIP, _serverPort),
				           Poco::Timespan(CLIENT_SOCKET_CONNECT_TIMEOUT_SECONDS, 0));
        }
        catch (Poco::TimeoutException &)
        {
            LOG_ERROR("Timeout connecting to the session server, exit");
            return false;
        }
        catch (Poco::Net::NetException &e)
        {
            LOG_ERROR(std::string("Error when connecting to the session server: [") + e.displayText() + "]"); 
            return false;
        }
        return true;
    }

    void UserImpl::startReceiving(User &user)
    {
        assert(_sock != NULL);
        _isReceiving = true;

        unsigned int headerLen = sizeof(sserver::message::Header);
        while (_isReceiving)
        {
            try  // we have to add try block here to detect the socket shutdown or close
            {
				if (sserver::Utils::recvBytes(*_sock, _recvBuff, headerLen))
				{
					sserver::message::Header *header = reinterpret_cast<sserver::message::Header*>(_recvBuff);
					char *realBuff = new char[header->getPktLen()];
					memcpy(realBuff, header, headerLen);

					if (sserver::Utils::recvBytes(*_sock, realBuff + headerLen, header->getPktLen() - headerLen))
					{
						switch (header->getPktType())
						{
						case sserver::message::PKT_TYPE_LOGIN_ACCEPT:
							{
								sserver::message::LoginAccept *pMsg = reinterpret_cast<sserver::message::LoginAccept*>(realBuff);
								_sessionId = pMsg->_header.getSessionId();

                                UserCallback::UserInfo info;
                                info._username = pMsg->getUsername();
                                info._userId = pMsg->getUserId();
                                info._userRole = sserver::server::USER_ROLE(pMsg->getUserRole());
                                info._regTime = pMsg->getRegTime();

                                char *p = reinterpret_cast<char*>(pMsg+1);
                                sserver::message::BinaryBlock *fp = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                p += sizeof(sserver::message::BinaryBlock);
                                info._fpLen = fp->getContentLen();
                                info._pFingerprint = p;
                                p += info._fpLen;

                                sserver::message::BinaryBlock *signature = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                p += sizeof(sserver::message::BinaryBlock);
                                info._signatureLen = signature->getContentLen();
                                info._pSignature = p;

								if (_userCallback)
								{
									_userCallback->onLoginAccpted(info);
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_LOGIN_REJECT:
							{
								sserver::message::LoginReject *pMsg = reinterpret_cast<sserver::message::LoginReject*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onLoginRejected(pMsg->getUsername());
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_ADD_PHARMACIST_CERT_TYPE_SUCC:
							{
								sserver::message::AddPharmacistCertTypeSucc *pMsg 
									= reinterpret_cast<sserver::message::AddPharmacistCertTypeSucc*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPharmacistCertTypeAdded(pMsg->getCertTypeName());
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_ADD_PHARMACIST_CERT_TYPE_FAIL:
							{
								sserver::message::AddPharmacistCertTypeFail *pMsg 
									= reinterpret_cast<sserver::message::AddPharmacistCertTypeFail*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPharmacistCertTypeAddFail(pMsg->getCertTypeName());
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACIST_CERT_TYPES:
							{
								sserver::message::PharmacistCertTypes *pMsg = 
									reinterpret_cast<sserver::message::PharmacistCertTypes*>(realBuff);
								unsigned int numOfCertTypes = pMsg->getNumOfCertTypes();
								std::vector<UserCallback::PharmacistCertTypeItem> result;

								sserver::message::PharmacistCertType_Item *item = 
									reinterpret_cast<sserver::message::PharmacistCertType_Item*>(pMsg+1);
								for (unsigned int i = 0; i < numOfCertTypes; ++i)
								{
									UserCallback::PharmacistCertTypeItem t;
									t._id = item->getId();
									t._certTypeName = item->getCertTypeName();
									result.push_back(t);
									++item;
								}

								if (_userCallback)
								{
									_userCallback->onPharmacistCertTypes(result);
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_ADD_STORE_SUCC:
							{
								sserver::message::AddStoreSucc *pMsg = reinterpret_cast<sserver::message::AddStoreSucc*>(realBuff);
								if (_userCallback)
								{
                                    char *p = reinterpret_cast<char*>(pMsg+1);
                                    sserver::message::BinaryBlock *name = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
									_userCallback->onStoreAdded(std::string(p, name->getContentLen()), pMsg->getStoreId());
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_ADD_STORE_FAIL:
							{
								sserver::message::AddStoreFail *pMsg = reinterpret_cast<sserver::message::AddStoreFail*>(realBuff);
								if (_userCallback)
								{
                                    char *p = reinterpret_cast<char*>(pMsg+1);
                                    sserver::message::BinaryBlock *name = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
									_userCallback->onStoreAddFail(std::string(p, name->getContentLen()));
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_DELETE_STORES_SUCC:
							{
								sserver::message::DeleteStoresSucc *pMsg = reinterpret_cast<sserver::message::DeleteStoresSucc*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onStoresDeleted();
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_DELETE_STORES_FAIL:
							{
								sserver::message::DeleteStoresFail *pMsg = reinterpret_cast<sserver::message::DeleteStoresFail*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onStoresDeleteFail();
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_STORES:
							{
								sserver::message::Stores *pMsg = reinterpret_cast<sserver::message::Stores*>(realBuff);
								Poco::UInt32 numOfStores = pMsg->getNumOfStores();
								std::vector<UserCallback::StoreItem> result;

								sserver::message::Store_Item *item = reinterpret_cast<sserver::message::Store_Item*>(pMsg+1);
								char *p = reinterpret_cast<char*>(item);

								for (unsigned int i = 0; i < numOfStores; ++i)
								{
									p += sizeof(sserver::message::Store_Item);

									UserCallback::StoreItem s;
									s._id = item->getId();

									sserver::message::BinaryBlock *binBlk = reinterpret_cast<sserver::message::BinaryBlock *>(p);
									p += sizeof(sserver::message::BinaryBlock);
									s._name = std::string(p, binBlk->getContentLen());
									p += binBlk->getContentLen();

									binBlk = reinterpret_cast<sserver::message::BinaryBlock *>(p);
									p += sizeof(sserver::message::BinaryBlock);
									s._addr = std::string(p, binBlk->getContentLen());
									p += binBlk->getContentLen();

                                    for (unsigned j = 0; j < item->getNumOfPatientAccts(); ++j)
                                    {
                                        binBlk = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                        p += sizeof(sserver::message::BinaryBlock);
                                        s._patientAccts.insert(std::string(p, binBlk->getContentLen()));
                                        p += binBlk->getContentLen();
                                    }

									result.push_back(s);
									item = reinterpret_cast<sserver::message::Store_Item*>(p);
								}

								if (_userCallback)
								{
									_userCallback->onStores(result);
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACIST_ADD_SUCC:
							{
								sserver::message::PharmacistAddSucc *pMsg = reinterpret_cast<sserver::message::PharmacistAddSucc*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPharmacistAdded(pMsg->getUsername());
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACIST_ADD_FAIL:
							{
								sserver::message::PharmacistAddFail *pMsg = reinterpret_cast<sserver::message::PharmacistAddFail*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPharmacistAddFail(pMsg->getUsername());
								}
								_syncEvt.set();
								break;
							}
                        case sserver::message::PKT_TYPE_DELETE_PHARMACISTS_SUCC:
                            {
                                sserver::message::DeletePharmacistsSucc *pMsg = reinterpret_cast<sserver::message::DeletePharmacistsSucc*>(realBuff);
                                if (_userCallback)
                                {
                                    _userCallback->onPharmacistsDeleted();
                                }
                                _syncEvt.set();
                                break;
                            }
                        case sserver::message::PKT_TYPE_DELETE_PHARMACISTS_FAIL:
                            {
                                sserver::message::DeletePharmacistsFail *pMsg = reinterpret_cast<sserver::message::DeletePharmacistsFail*>(realBuff);
                                if (_userCallback)
                                {
                                    _userCallback->onPharmacistsDeleteFail();
                                }
                                _syncEvt.set();
                                break;
                            }
						case sserver::message::PKT_TYPE_PHARMACISTS_BRIEF:
							{
								sserver::message::PharmacistsBrief *brief = reinterpret_cast<sserver::message::PharmacistsBrief*>(realBuff);
								Poco::UInt32 numOfPharmacists = brief->getNumOfPharmacists();
								std::vector<UserCallback::PharmacistBriefItem> result;

								sserver::message::PharmacistsBrief_Item *item = reinterpret_cast<sserver::message::PharmacistsBrief_Item*>(brief+1);
								char *p = reinterpret_cast<char*>(item);

								for (unsigned int i = 0; i < numOfPharmacists; ++i)
								{
									p += sizeof(sserver::message::PharmacistsBrief_Item);

									UserCallback::PharmacistBriefItem b;
									b._userId = item->getUserId();
									b._realName = item->getRealName();
									b._certTypeId = item->getCertTypeId();
									b._status = sserver::server::USER_STATUS(item->getStatus());
									if (item->getHasPhoto())
									{
										sserver::message::BinaryBlock *photo = reinterpret_cast<sserver::message::BinaryBlock *>(p);
										p += sizeof(sserver::message::BinaryBlock);
										b._photoLen = photo->getContentLen();
										b._pPhoto = p;
										p += b._photoLen;
									}
									else
									{
										b._photoLen = 0;
										b._pPhoto = NULL;
									}

									result.push_back(b);
									item = reinterpret_cast<sserver::message::PharmacistsBrief_Item*>(p);
								}

								if (_userCallback)
								{
									_userCallback->onPharmacistsBrief(result);
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACISTS_DETAIL:
							{
								sserver::message::PharmacistsDetail *detail = reinterpret_cast<sserver::message::PharmacistsDetail*>(realBuff);
								Poco::UInt32 numOfPharmacists = detail->getNumOfPharmacists();
								std::vector<UserCallback::PharmacistDetailItem> result;

								sserver::message::PharmacistsDetail_Item *item = reinterpret_cast<sserver::message::PharmacistsDetail_Item*>(detail+1);
								char *p = reinterpret_cast<char*>(item);

								for (unsigned int i = 0; i < numOfPharmacists; ++i)
								{
									p += sizeof(sserver::message::PharmacistsDetail_Item);

									UserCallback::PharmacistDetailItem d;
									d._userId = item->getUserId();
                                    d._username = item->getUsername();
									d._realName = item->getRealName();
									d._certTypeId = item->getCertTypeId();
									d._status = sserver::server::USER_STATUS(item->getStatus());
									
                                    sserver::message::BinaryBlock *shenfenzheng = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._shenfenzheng = std::string(p, shenfenzheng->getContentLen());
                                    p += shenfenzheng->getContentLen();
                                    
                                    sserver::message::BinaryBlock *cert = reinterpret_cast<sserver::message::BinaryBlock *>(p);
									p += sizeof(sserver::message::BinaryBlock);
									d._certLen = cert->getContentLen();
									d._pCert = p;
									p += d._certLen;

									sserver::message::BinaryBlock *fp = reinterpret_cast<sserver::message::BinaryBlock *>(p);
									p += sizeof(sserver::message::BinaryBlock);
									d._fpLen = fp->getContentLen();
									d._pFingerprint = p;
									p += d._fpLen;

                                    sserver::message::BinaryBlock *signature = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._signatureLen = signature->getContentLen();
                                    d._pSignature = p;
                                    p += d._signatureLen;

									Poco::UInt8 numOfCoveredStores = item->getNumOfCoveredStores();
									for (unsigned int i = 0; i < numOfCoveredStores; ++i)
									{
										Poco::UInt32 *pNetVal = reinterpret_cast<Poco::UInt32*>(p); 
										d._coveredStoreIds.insert(sserver::Utils::readNumericField<Poco::UInt32>(*pNetVal));
										p += sizeof(Poco::UInt32);
									}

                                    sserver::message::BinaryBlock *intro = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._introductionLen = intro->getContentLen();
                                    d._pIntroduction = p;
									p += d._introductionLen;

                                    sserver::message::BinaryBlock *photo = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._photoLen = photo->getContentLen();
                                    d._pPhoto = p;
                                    p += d._photoLen;
									
									result.push_back(d);
									item = reinterpret_cast<sserver::message::PharmacistsDetail_Item*>(p);
								}

								if (_userCallback)
								{
									_userCallback->onPharmacistsDetail(result);
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PATIENT_ADD_SUCC:
							{
								sserver::message::PatientAddSucc *pMsg = reinterpret_cast<sserver::message::PatientAddSucc*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPatientAdded(pMsg->getUsername());
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PATIENT_ADD_FAIL:
							{
								sserver::message::PatientAddFail *pMsg = reinterpret_cast<sserver::message::PatientAddFail*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPatientAddFail(pMsg->getUsername());
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACISTS_DETAIL_OF_CUR_STORE:
							{
								sserver::message::PharmacistsDetailOfCurrentStore 
									*detail = reinterpret_cast<sserver::message::PharmacistsDetailOfCurrentStore*>(realBuff);

								Poco::UInt32 numOfPharmacists = detail->getNumOfPharmacists();
								std::vector<UserCallback::PharmacistOfCurrentStore> result;

								sserver::message::PharmacistsDetailOfCurrentStore_Item 
									*item = reinterpret_cast<sserver::message::PharmacistsDetailOfCurrentStore_Item*>(detail+1);
								char *p = reinterpret_cast<char*>(item);

								for (unsigned int i = 0; i < numOfPharmacists; ++i)
								{
									p += sizeof(sserver::message::PharmacistsDetailOfCurrentStore_Item);

									UserCallback::PharmacistOfCurrentStore d;
									d._realName = item->getRealName();
									d._certTypeId = item->getCertTypeId();

                                    sserver::message::BinaryBlock *intro = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._introductionLen = intro->getContentLen();
                                    d._pIntroduction = p;
                                    p += d._introductionLen;

                                    sserver::message::BinaryBlock *photo = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._photoLen = photo->getContentLen();
                                    d._pPhoto = p;
                                    p += d._photoLen;

									result.push_back(d);
									item = reinterpret_cast<sserver::message::PharmacistsDetailOfCurrentStore_Item*>(p);
								}

								if (_userCallback)
								{
									_userCallback->onPharmacistsDetailOfCurStore(result);
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACIST_REQUEST_SUCC:
							{
								// -----------------------------------------------
								try
								{
									_videoSock->connect(Poco::Net::SocketAddress(_serverIP, _serverPort),
										                Poco::Timespan(CLIENT_SOCKET_CONNECT_TIMEOUT_SECONDS, 0));
								}
								catch (Poco::TimeoutException &)
								{
									LOG_ERROR("Timeout establishing the video socket");
								}
								catch (Poco::Net::NetException &e)
								{
									LOG_ERROR(std::string("Error when establishing the video socket: [") + e.displayText() + "]"); 
								}

								// -----------------------------------------------
								try
								{
									_audioSock->connect(Poco::Net::SocketAddress(_serverIP, _serverPort),
										                Poco::Timespan(CLIENT_SOCKET_CONNECT_TIMEOUT_SECONDS, 0));
								}
								catch (Poco::TimeoutException &)
								{
									LOG_ERROR("Timeout establishing the audio socket");
								}
								catch (Poco::Net::NetException &e)
								{
									LOG_ERROR(std::string("Error when establishing the audio socket: [") + e.displayText() + "]"); 
								}

								// -----------------------------------------------
								sserver::message::PharmacistRequestSucc *pMsg = 
									reinterpret_cast<sserver::message::PharmacistRequestSucc*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPharmacistRequestSucc(pMsg->getPharmacistSessionId());
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACIST_REQUEST_FAIL:
							{
								sserver::message::PharmacistRequestFail *pMsg = 
									reinterpret_cast<sserver::message::PharmacistRequestFail*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPharmacistRequestFail();
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PATIENT_CONNECTED:
							{
								// -----------------------------------------------
								try
								{
									_videoSock->connect(Poco::Net::SocketAddress(_serverIP, _serverPort),
										                Poco::Timespan(CLIENT_SOCKET_CONNECT_TIMEOUT_SECONDS, 0));
								}
								catch (Poco::TimeoutException &)
								{
									LOG_ERROR("Timeout establishing the video socket");
								}
								catch (Poco::Net::NetException &e)
								{
									LOG_ERROR(std::string("Error when establishing the video socket: [") + e.displayText() + "]"); 
								}

								// -----------------------------------------------
								try
								{
									_audioSock->connect(Poco::Net::SocketAddress(_serverIP, _serverPort),
										                Poco::Timespan(CLIENT_SOCKET_CONNECT_TIMEOUT_SECONDS, 0));
								}
								catch (Poco::TimeoutException &)
								{
									LOG_ERROR("Timeout establishing the audio socket");
								}
								catch (Poco::Net::NetException &e)
								{
									LOG_ERROR(std::string("Error when establishing the audio socket: [") + e.displayText() + "]"); 
								}
								
								// -----------------------------------------------
								sserver::message::PatientConnected *pMsg = reinterpret_cast<sserver::message::PatientConnected*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPatientConnected(pMsg->getPatientSessionId());
								}
								break;
							}
						case sserver::message::PKT_TYPE_DATA_TRANSFER_PRESCRIPTION:
							{
								sserver::message::Prescription *pMsg = reinterpret_cast<sserver::message::Prescription*>(realBuff);
								sserver::message::BinaryBlock *binBlk = reinterpret_cast<sserver::message::BinaryBlock*>(pMsg+1);
								User::Prescription prescription;
								prescription._dataLen = binBlk->getContentLen();
								prescription._pData = reinterpret_cast<char*>(binBlk+1);
								if (_userCallback)
								{
									_userCallback->onPrescription(prescription);
								}
								break;
							}
						case sserver::message::PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG_SUCC:
							{
								sserver::message::AddDealOfPrescriptionDrugSucc *pMsg = 
									reinterpret_cast<sserver::message::AddDealOfPrescriptionDrugSucc*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onDealOfPrescriptionDrugAdded();
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG_FAIL:
							{
								sserver::message::AddDealOfPrescriptionDrugFail *pMsg = 
									reinterpret_cast<sserver::message::AddDealOfPrescriptionDrugFail*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onDealOfPrescriptionDrugAddFail();
								}
								_syncEvt.set();
								break;
							}
                        case sserver::message::PKT_TYPE_DEALS_OF_PRESCRIPTION_DRUG_DETAIL:
                            {
                                sserver::message::DealsOfPrescriptionDrugDetail *detail 
                                    = reinterpret_cast<sserver::message::DealsOfPrescriptionDrugDetail*>(realBuff);
                                Poco::UInt32 numOfDeals = detail->getNumOfDeals();
                                std::vector<UserCallback::DealsOfPrescriptionDrugDetailItem> result;

                                sserver::message::DealsOfPrescriptionDrugDetail_Item *item 
                                    = reinterpret_cast<sserver::message::DealsOfPrescriptionDrugDetail_Item*>(detail+1);
                                char *p = reinterpret_cast<char*>(item);

                                for (unsigned int i = 0; i < numOfDeals; ++i)
                                {
                                    p += sizeof(sserver::message::DealsOfPrescriptionDrugDetail_Item);

                                    UserCallback::DealsOfPrescriptionDrugDetailItem d;
                                    d._dealId = item->getDealId();
                                    d._buyerAge = item->getBuyerAge();
                                    d._buyerIsMale = item->getBuyerIsMale();
                                    d._time = item->getTime();

                                    sserver::message::BinaryBlock *buyerName = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._buyerName = std::string(p, buyerName->getContentLen());
                                    p += buyerName->getContentLen();

                                    sserver::message::BinaryBlock *drugHuoHao = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugHuoHao = std::string(p, drugHuoHao->getContentLen());
                                    p += drugHuoHao->getContentLen();

                                    sserver::message::BinaryBlock *drugMingCheng = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugMingCheng = std::string(p, drugMingCheng->getContentLen());
                                    p += drugMingCheng->getContentLen();

                                    sserver::message::BinaryBlock *drugPiHao = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugPiHao = std::string(p, drugPiHao->getContentLen());
                                    p += drugPiHao->getContentLen();

                                    sserver::message::BinaryBlock *drugGuiGe = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugGuiGe = std::string(p, drugGuiGe->getContentLen());
                                    p += drugGuiGe->getContentLen();

                                    sserver::message::BinaryBlock *drugJiLiang = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugJiLiang = std::string(p, drugJiLiang->getContentLen());
                                    p += drugJiLiang->getContentLen();

                                    sserver::message::BinaryBlock *drugShengChanChangJia = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugShengChanChangJia = std::string(p, drugShengChanChangJia->getContentLen());
                                    p += drugShengChanChangJia->getContentLen();

                                    sserver::message::BinaryBlock *drugChuFangLaiYuan = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugChuFangLaiYuan = std::string(p, drugChuFangLaiYuan->getContentLen());
                                    p += drugChuFangLaiYuan->getContentLen();

                                    result.push_back(d);
                                    item = reinterpret_cast<sserver::message::DealsOfPrescriptionDrugDetail_Item*>(p);
                                }

                                if (_userCallback)
                                {
                                    _userCallback->onDealsOfPrescriptionDrugDetail(result);
                                }
                                _syncEvt.set();
                                break;
                            }
                        case sserver::message::PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG_SUCC:
                            {
                                sserver::message::AddDealOfSpecialDrugSucc *pMsg = 
                                    reinterpret_cast<sserver::message::AddDealOfSpecialDrugSucc*>(realBuff);
                                if (_userCallback)
                                {
                                    _userCallback->onDealOfSpecialDrugAdded();
                                }
                                _syncEvt.set();
                                break;
                            }
                        case sserver::message::PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG_FAIL:
                            {
                                sserver::message::AddDealOfSpecialDrugFail *pMsg = 
                                    reinterpret_cast<sserver::message::AddDealOfSpecialDrugFail*>(realBuff);
                                if (_userCallback)
                                {
                                    _userCallback->onDealOfSpecialDrugAddFail();
                                }
                                _syncEvt.set();
                                break;
                            }
                        case sserver::message::PKT_TYPE_DEALS_OF_SPECIAL_DRUG_DETAIL:
                            {
                                sserver::message::DealsOfSpecialDrugDetail *detail 
                                    = reinterpret_cast<sserver::message::DealsOfSpecialDrugDetail*>(realBuff);
                                Poco::UInt32 numOfDeals = detail->getNumOfDeals();
                                std::vector<UserCallback::DealsOfSpecialDrugDetailItem> result;

                                sserver::message::DealsOfSpecialDrugDetail_Item *item 
                                    = reinterpret_cast<sserver::message::DealsOfSpecialDrugDetail_Item*>(detail+1);
                                char *p = reinterpret_cast<char*>(item);

                                for (unsigned int i = 0; i < numOfDeals; ++i)
                                {
                                    p += sizeof(sserver::message::DealsOfSpecialDrugDetail_Item);

                                    UserCallback::DealsOfSpecialDrugDetailItem d;
                                    d._dealId = item->getDealId();
                                    d._buyerAge = item->getBuyerAge();
                                    d._buyerIsMale = item->getBuyerIsMale();
                                    d._time = item->getTime();

                                    sserver::message::BinaryBlock *buyerName = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._buyerName = std::string(p, buyerName->getContentLen());
                                    p += buyerName->getContentLen();

                                    sserver::message::BinaryBlock *buyerShenFenZheng = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._buyerShenFenZheng = std::string(p, buyerShenFenZheng->getContentLen());
                                    p += buyerShenFenZheng->getContentLen();

                                    sserver::message::BinaryBlock *drugHuoHao = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugHuoHao = std::string(p, drugHuoHao->getContentLen());
                                    p += drugHuoHao->getContentLen();

                                    sserver::message::BinaryBlock *drugMingCheng = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugMingCheng = std::string(p, drugMingCheng->getContentLen());
                                    p += drugMingCheng->getContentLen();

                                    sserver::message::BinaryBlock *drugPiHao = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugPiHao = std::string(p, drugPiHao->getContentLen());
                                    p += drugPiHao->getContentLen();

                                    sserver::message::BinaryBlock *drugGuiGe = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugGuiGe = std::string(p, drugGuiGe->getContentLen());
                                    p += drugGuiGe->getContentLen();

                                    sserver::message::BinaryBlock *drugJiLiang = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugJiLiang = std::string(p, drugJiLiang->getContentLen());
                                    p += drugJiLiang->getContentLen();

                                    sserver::message::BinaryBlock *drugGouMaiShuLiang = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugGouMaiShuLiang = std::string(p, drugGouMaiShuLiang->getContentLen());
                                    p += drugGouMaiShuLiang->getContentLen();

                                    sserver::message::BinaryBlock *drugShengChanChangJia = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugShengChanChangJia = std::string(p, drugShengChanChangJia->getContentLen());
                                    p += drugShengChanChangJia->getContentLen();

                                    sserver::message::BinaryBlock *drugChuFangLaiYuan = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._drugChuFangLaiYuan = std::string(p, drugChuFangLaiYuan->getContentLen());
                                    p += drugChuFangLaiYuan->getContentLen();

                                    result.push_back(d);
                                    item = reinterpret_cast<sserver::message::DealsOfSpecialDrugDetail_Item*>(p);
                                }

                                if (_userCallback)
                                {
                                    _userCallback->onDealsOfSpecialDrugDetail(result);
                                }
                                _syncEvt.set();
                                break;
                            }
						case sserver::message::PKT_TYPE_CONSULTING_DETAILS:
							{
								sserver::message::ConsultingDetails *detail 
									= reinterpret_cast<sserver::message::ConsultingDetails*>(realBuff);
								Poco::UInt32 numOfItems = detail->getNumOfItems();
								std::vector<UserCallback::ConsultingDetailItem> result;

								sserver::message::ConsultingDetails_Item *item 
									= reinterpret_cast<sserver::message::ConsultingDetails_Item*>(detail+1);
								char *p = reinterpret_cast<char*>(item);

								for (unsigned int i = 0; i < numOfItems; ++i)
								{
									p += sizeof(sserver::message::ConsultingDetails_Item);

									UserCallback::ConsultingDetailItem d;
									d._pharmacistId = item->getPharmacistId();
									d._patientId = item->getPatientId();
									d._startTime = item->getStartTime();
									d._endTime = item->getEndTime();
									
									sserver::message::BinaryBlock *videoLoc1 = reinterpret_cast<sserver::message::BinaryBlock *>(p);
									p += sizeof(sserver::message::BinaryBlock);
									d._pharmacistVideoLoc = std::string(p, videoLoc1->getContentLen());
									p += videoLoc1->getContentLen();

                                    sserver::message::BinaryBlock *videoLoc2 = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._patientVideoLoc = std::string(p, videoLoc2->getContentLen());
                                    p += videoLoc2->getContentLen();

									sserver::message::BinaryBlock *audioLoc1 = reinterpret_cast<sserver::message::BinaryBlock *>(p);
									p += sizeof(sserver::message::BinaryBlock);
									d._pharmacistAudioLoc = std::string(p, audioLoc1->getContentLen());
									p += audioLoc1->getContentLen();

                                    sserver::message::BinaryBlock *audioLoc2 = reinterpret_cast<sserver::message::BinaryBlock *>(p);
                                    p += sizeof(sserver::message::BinaryBlock);
                                    d._patientAudioLoc = std::string(p, audioLoc2->getContentLen());
                                    p += audioLoc2->getContentLen();

									Poco::UInt32 numOfPrescriptions = item->getNumOfPrescriptions();
									for (unsigned int j = 0; j < numOfPrescriptions; ++j)
									{
										sserver::message::BinaryBlock *prescriptionLoc = reinterpret_cast<sserver::message::BinaryBlock *>(p);
										p += sizeof(sserver::message::BinaryBlock);
										d._prescriptionLocs.push_back(std::string(p, prescriptionLoc->getContentLen()));
										p += prescriptionLoc->getContentLen();
									}

									result.push_back(d);
									item = reinterpret_cast<sserver::message::ConsultingDetails_Item*>(p);
								}

								if (_userCallback)
								{
									_userCallback->onConsultingDetails(result);
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACIST_ACTIVITY:
							{
								sserver::message::PharmacistActivity *detail 
									= reinterpret_cast<sserver::message::PharmacistActivity*>(realBuff);
								Poco::UInt32 numOfItems = detail->getNumOfItems();
								
								std::vector<std::string> result;
								char *p = reinterpret_cast<char*>(detail+1);

								for (unsigned int i = 0; i < numOfItems; ++i)
								{
									sserver::message::BinaryBlock *time = reinterpret_cast<sserver::message::BinaryBlock *>(p);
									p += sizeof(sserver::message::BinaryBlock);
									result.push_back(std::string(p, time->getContentLen()));
									p += time->getContentLen();
								}

								if (_userCallback)
								{
									_userCallback->onPharmacistActivityList(result);
								}
								_syncEvt.set();
								break;
							}
						case sserver::message::PKT_TYPE_PATIENT_QUIT_CONVERSATION:
							{
								// only shutdown the video and audio sockets
								sserver::Utils::closeSocket(*_videoSock);
								sserver::Utils::closeSocket(*_audioSock);
								
								sserver::message::PatientQuitService *pMsg = reinterpret_cast<sserver::message::PatientQuitService*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPatientQuitService(pMsg->getPatientSessionId());
								}                           
								break;
							}
						case sserver::message::PKT_TYPE_PHARMACIST_QUIT_CONVERSATION:
							{
								// only shutdown the video and audio sockets
								sserver::Utils::closeSocket(*_videoSock);
								sserver::Utils::closeSocket(*_audioSock);

								sserver::message::PharmacistQuitService *pMsg = reinterpret_cast<sserver::message::PharmacistQuitService*>(realBuff);
								if (_userCallback)
								{
									_userCallback->onPharmacistQuitService(pMsg->getPharmacistSessionId());
								}
								break;
							}
						default:
							break;
						}
					}
					delete [] realBuff;
				}
				else
				{
					// here we quit the receiving loop actively
					sserver::Utils::closeSocket(*_sock);
					_isReceiving = false;
				}
            }
            catch (...)
            {
				// here we quit the receiving loop actively
				sserver::Utils::closeSocket(*_sock);
				_isReceiving = false;
            }
        }
    }

	void UserImpl::startReceivingVideo(User &user)
	{
		assert(_videoSock != NULL);
		_isReceivingVideo = true;

		unsigned int headerLen = sizeof(sserver::message::Header);
		while (_isReceivingVideo)
		{
            try  // we have to add try block here to detect the socket shutdown or close
            {
                if (sserver::Utils::recvBytes(*_videoSock, _videoRecvBuff, headerLen))
                {
                    sserver::message::Header *header = reinterpret_cast<sserver::message::Header*>(_videoRecvBuff);
                    char *realBuff = new char[header->getPktLen()];
                    memcpy(realBuff, header, headerLen);

                    if (sserver::Utils::recvBytes(*_videoSock, realBuff + headerLen, header->getPktLen() - headerLen))
                    {
                        switch (header->getPktType())
                        {
                        case sserver::message::PKT_TYPE_DATA_TRANSFER_VIDEO:
                            {
                                sserver::message::Video *pMsg = reinterpret_cast<sserver::message::Video*>(realBuff);
                                sserver::message::BinaryBlock *binBlk = reinterpret_cast<sserver::message::BinaryBlock*>(pMsg+1);
                                User::Video video;
                                video._dataLen = binBlk->getContentLen();
                                video._pData = reinterpret_cast<char*>(binBlk+1);
                                if (_userCallback)
                                {
                                    _userCallback->onVideo(video);
                                }
                                break;
                            }
                        default:
                            break;
                        }
                    }
                    delete [] realBuff;
                }
				else
				{
					// here we quit the receiving loop actively
					sserver::Utils::closeSocket(*_videoSock);
					_isReceivingVideo = false;
				}
            }
            catch (...)
            {
                // here we quit the receiving loop actively
                sserver::Utils::closeSocket(*_videoSock);
				_isReceivingVideo = false;
            }
		}
	}

	void UserImpl::startReceivingAudio(User &user)
	{
        assert(_audioSock != NULL);
        _isReceivingAudio = true;

        unsigned int headerLen = sizeof(sserver::message::Header);
        while (_isReceivingAudio)
        {
            try  // we have to add try block here to detect the socket shutdown or close
            {
                if (sserver::Utils::recvBytes(*_audioSock, _audioRecvBuff, headerLen))
                {
                    sserver::message::Header *header = reinterpret_cast<sserver::message::Header*>(_audioRecvBuff);
                    char *realBuff = new char[header->getPktLen()];
                    memcpy(realBuff, header, headerLen);

                    if (sserver::Utils::recvBytes(*_audioSock, realBuff + headerLen, header->getPktLen() - headerLen))
                    {
                        switch (header->getPktType())
                        {
                        case sserver::message::PKT_TYPE_DATA_TRANSFER_AUDIO:
                            {
                                sserver::message::Audio *pMsg = reinterpret_cast<sserver::message::Audio*>(realBuff);
                                sserver::message::BinaryBlock *binBlk = reinterpret_cast<sserver::message::BinaryBlock*>(pMsg+1);
                                User::Audio audio;
                                audio._dataLen = binBlk->getContentLen();
                                audio._pData = reinterpret_cast<char*>(binBlk+1);
                                if (_userCallback)
                                {
                                    _userCallback->onAudio(audio);
                                }
                                break;
                            }
                        default:
                            break;
                        }
                    }
                    delete [] realBuff;
                }
				else
				{
					// here we quit the receiving loop actively
					sserver::Utils::closeSocket(*_audioSock);
					_isReceivingAudio = false;
				}
            }
            catch (...)
            {
                // here we quit the receiving loop actively
				sserver::Utils::closeSocket(*_audioSock);
				_isReceivingAudio = false;
            }
        }
	}

    void UserImpl::stopReceiving()
    {
        _isReceiving = false;
    }

	void UserImpl::stopReceivingVideo()
	{
		_isReceivingVideo = false;
	}

	void UserImpl::stopReceivingAudio()
	{
		_isReceivingAudio = false;
	}

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_Login(const char *pFingerprint, unsigned int fpLen)
    {
        sserver::message::LoginRequest msg;
        msg._header.setPktLen(sizeof(msg) + 
			                  sizeof(sserver::message::BinaryBlock) + fpLen);
        msg.setUsername(_username);
        msg.setPassword(_password);

		if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

		sserver::message::BinaryBlock binBlk;
		binBlk.setContentLen(fpLen);
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, pFingerprint, fpLen)) return false;

        return true;
    }

    bool UserImpl::req_Login(const char *pFingerprint, unsigned int fpLen)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_Login(pFingerprint, fpLen);
    }

    bool UserImpl::req_Login_Sync(const char *pFingerprint, unsigned int fpLen)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_Login(pFingerprint, fpLen);
        _syncEvt.wait();
        return rst;
    }

	// ------------------------------------------------------------------------------------------
	bool UserImpl::send_RefreshFingerprint()
	{
		assert(_sessionId != 0);
		Poco::FastMutex::ScopedLock lock(_sendMutex);

		sserver::message::RefreshFingerprint msg;
		msg._header.setSessionId(_sessionId);
		msg._header.setPktLen(sizeof(msg));

		return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
	}

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_AddPharmacistCertType(const std::string &certTypeName)
    {
        assert(_sessionId != 0);

        sserver::message::AddPharmacistCertType msg;
        msg._header.setPktLen(sizeof(msg));
        msg._header.setSessionId(_sessionId);
        msg.setCertTypeName(certTypeName);
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

    bool UserImpl::req_AddPharmacistCertType(const std::string &certTypeName)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_AddPharmacistCertType(certTypeName);
    }

    bool UserImpl::req_AddPharmacistCertType_Sync(const std::string &certTypeName)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_AddPharmacistCertType(certTypeName);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_ListPharmacistCertTypes()
    {
        assert(_sessionId != 0);

        sserver::message::ListPharmacistCertTypes msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(msg));
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }
    
    bool UserImpl::req_ListPharmacistCertTypes()
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_ListPharmacistCertTypes();
    }

    bool UserImpl::req_ListPharmacistCertTypes_Sync()
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_ListPharmacistCertTypes();
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_AddStore(const User::StoreRegInfo &info)
    {
        assert(_sessionId != 0);

        sserver::message::AddStore msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(sserver::message::AddStore) +
			info._pharmacistIds.size() * sizeof(Poco::UInt32) +
            sizeof(sserver::message::BinaryBlock) + info._name.length() +
            sizeof(sserver::message::BinaryBlock) + info._addr.length());

        msg.setUpdateStoreId(info._updateStoreId);
        msg.setNumOfPharmacists(info._pharmacistIds.size());

        if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

		std::set<unsigned int>::const_iterator iter = info._pharmacistIds.begin();
		while (iter != info._pharmacistIds.end())
		{
			Poco::UInt32 netVal;
			sserver::Utils::writeNumericField<Poco::UInt32>(netVal, *iter);
			if (!sserver::Utils::sendBytes(*_sock, &netVal, sizeof(netVal))) return false;
			++iter;
		}

        sserver::message::BinaryBlock binBlk;
        binBlk.setContentLen(info._name.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, info._name.c_str(), info._name.length())) return false;
        binBlk.setContentLen(info._addr.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        return sserver::Utils::sendBytes(*_sock, info._addr.c_str(), info._addr.length());
    }

    bool UserImpl::req_AddStore(const User::StoreRegInfo &info)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_AddStore(info);
    }

    bool UserImpl::req_AddStore_Sync(const User::StoreRegInfo &info)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_AddStore(info);
        _syncEvt.wait();
        return rst;
    }

	// ------------------------------------------------------------------------------------------
	bool UserImpl::_req_DeleteStores(const std::set<unsigned int> &storeIds)
	{
		assert(_sessionId != 0);

		sserver::message::DeleteStores msg;
		msg._header.setSessionId(_sessionId);
		msg._header.setPktLen(sizeof(sserver::message::DeleteStores) +
			storeIds.size() * sizeof(Poco::UInt32));
		msg.setNumOfStores(storeIds.size());

		if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

		std::set<unsigned int>::const_iterator iter = storeIds.begin();
		while (iter != storeIds.end())
		{
			Poco::UInt32 netVal;
			sserver::Utils::writeNumericField<Poco::UInt32>(netVal, *iter);
			if (!sserver::Utils::sendBytes(*_sock, &netVal, sizeof(netVal))) return false;
			++iter;
		}

		return true;
	}

	bool UserImpl::req_DeleteStores(const std::set<unsigned int> &storeIds)
	{
		assert(_sock != NULL);
		Poco::FastMutex::ScopedLock lock(_sendMutex);
		return _req_DeleteStores(storeIds);
	}

	bool UserImpl::req_DeleteStores_Sync(const std::set<unsigned int> &storeIds)
	{
		assert(_sock != NULL);
		Poco::FastMutex::ScopedLock lock(_sendMutex);

		_syncEvt.reset();
		bool rst = _req_DeleteStores(storeIds);
		_syncEvt.wait();
		return rst;
	}

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_ListStores()
    {
        assert(_sessionId != 0);

        sserver::message::ListStroes msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(msg));
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

    bool UserImpl::req_ListStores()
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_ListStores();
    }

    bool UserImpl::req_ListStores_Sync()
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_ListStores();
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_AddPharmacist(const User::PharmacistRegInfo &info)
    {
        assert(_sessionId != 0);

        sserver::message::PharmacistAdd msg;
		msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(sserver::message::PharmacistAdd) +
			6 * sizeof(sserver::message::BinaryBlock) +
			info._shenfenzheng.length() +
			info._certLen + 
			info._fpLen +
            info._signatureLen +
			info._introductionLen +
			info._photoLen);

		msg.setUsername(info._username);
		msg.setPassword(info._password);
		msg.setRealName(info._realName);
		msg.setCertTypeId(info._certTypeId);
		msg.setUpdatePharmacistId(info._updatePharmacistId);
		
		if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

		sserver::message::BinaryBlock binBlk;
		binBlk.setContentLen(info._shenfenzheng.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, info._shenfenzheng.c_str(), info._shenfenzheng.length())) return false;

		binBlk.setContentLen(info._certLen);
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, info._pCert, info._certLen)) return false;

		binBlk.setContentLen(info._fpLen);
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, info._pFingerprint, info._fpLen)) return false;

        binBlk.setContentLen(info._signatureLen);
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, info._pSignature, info._signatureLen)) return false;

		binBlk.setContentLen(info._introductionLen);
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, info._pIntroduction, info._introductionLen)) return false;

		binBlk.setContentLen(info._photoLen);
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, info._pPhoto, info._photoLen)) return false;

        return true;
    }

    bool UserImpl::req_AddPharmacist(const User::PharmacistRegInfo &info)
    {
        assert(_sock != NULL);
        if (info._certLen == 0 || info._fpLen == 0) return false;

        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_AddPharmacist(info);
    }

    bool UserImpl::req_AddPharmacist_Sync(const User::PharmacistRegInfo &info)
    {
        assert(_sock != NULL);
        if (info._certLen == 0 || info._fpLen == 0) return false;

        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_AddPharmacist(info);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_DeletePharmacists(const std::set<unsigned int> &pharmacistIds)
    {
        assert(_sessionId != 0);

        sserver::message::DeletePharmacists msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(sserver::message::DeletePharmacists) +
            pharmacistIds.size() * sizeof(Poco::UInt32));
        msg.setNumOfPharmacists(pharmacistIds.size());

        if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

        std::set<unsigned int>::const_iterator iter = pharmacistIds.begin();
        while (iter != pharmacistIds.end())
        {
            Poco::UInt32 netVal;
            sserver::Utils::writeNumericField<Poco::UInt32>(netVal, *iter);
            if (!sserver::Utils::sendBytes(*_sock, &netVal, sizeof(netVal))) return false;
            ++iter;
        }

        return true;
    }

    bool UserImpl::req_DeletePharmacists(const std::set<unsigned int> &pharmacistIds)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_DeletePharmacists(pharmacistIds);
    }

    bool UserImpl::req_DeletePharmacists_Sync(const std::set<unsigned int> &pharmacistIds)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_DeletePharmacists(pharmacistIds);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_ListPharmacists(bool includePhoto)
    {
        assert(_sessionId != 0);

        sserver::message::PharmacistsBriefList msg;
        msg._header.setPktLen(sizeof(msg));
        msg._header.setSessionId(_sessionId);
		msg.setIncludePhoto(includePhoto);
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

    bool UserImpl::req_ListPharmacists(bool includePhoto)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_ListPharmacists(includePhoto);
    }

    bool UserImpl::req_ListPharmacists_Sync(bool includePhoto)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_ListPharmacists(includePhoto);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
	bool UserImpl::_req_ListPharmacistsDetail(unsigned int userId)
    {
        assert(_sessionId != 0);

        sserver::message::PharmacistsDetailList msg;
        msg._header.setPktLen(sizeof(msg));
        msg._header.setSessionId(_sessionId);
		msg.setUserId(userId);
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

	bool UserImpl::req_ListPharmacistsDetail(unsigned int userId)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_ListPharmacistsDetail(userId);
    }

	bool UserImpl::req_ListPharmacistsDetail_Sync(unsigned int userId)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_ListPharmacistsDetail(userId);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_AddStoreAccount(const User::StoreAccountRegInfo &info)
    {
        assert(_sessionId != 0);

        sserver::message::PatientAdd msg;
        msg._header.setPktLen(sizeof(msg));
        msg._header.setSessionId(_sessionId);
        msg.setIsUpdate(info._isUpdate);
        msg.setUsername(info._username);
        msg.setPassword(info._password);
        msg.setStoreId(info._storeId);
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

    bool UserImpl::req_AddStoreAccount(const User::StoreAccountRegInfo &info)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_AddStoreAccount(info);
    }

    bool UserImpl::req_AddStoreAccount_Sync(const User::StoreAccountRegInfo &info)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_AddStoreAccount(info);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_PharmacistsDetailOfCurStore()
    {
        assert(_sessionId != 0);

        sserver::message::GetPharmacistsDetailOfCurrentStore msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(msg));
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

    bool UserImpl::req_PharmacistsDetailOfCurStore()
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_PharmacistsDetailOfCurStore();
    }

    bool UserImpl::req_PharmacistsDetailOfCurStore_Sync()
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_PharmacistsDetailOfCurStore();
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_Pharmacist(unsigned int certTypeId, bool usePool)
    {
        assert(_sessionId != 0);

        sserver::message::PharmacistRequest msg;
        msg._header.setPktLen(sizeof(msg));
        msg._header.setSessionId(_sessionId);
		msg.setCertTypeId(certTypeId);
        msg.setUsePool(usePool);
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

    bool UserImpl::req_Pharmacist(unsigned int certTypeId, bool usePool)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_Pharmacist(certTypeId, usePool);
    }

    bool UserImpl::req_Pharmacist_Sync(unsigned int certTypeId, bool usePool)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_Pharmacist(certTypeId, usePool);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::send_Prescription(const User::Prescription &prescription)
    {
        assert(_sessionId != 0);
        assert((_sock != NULL) || (prescription._dataLen != 0) || (prescription._pData != NULL));
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        sserver::message::Prescription msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(msg) + sizeof(sserver::message::BinaryBlock) + prescription._dataLen);

        if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;
        sserver::message::BinaryBlock binBlk;
        binBlk.setContentLen(prescription._dataLen);
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        return sserver::Utils::sendBytes(*_sock, prescription._pData, prescription._dataLen);
    }

	// ------------------------------------------------------------------------------------------
	bool UserImpl::send_Video(const User::Video &videoPac)
	{
		assert(_sessionId != 0);
		assert((_videoSock != NULL) || (videoPac._dataLen != 0) || (videoPac._pData != NULL));

		// don't need lock here

		sserver::message::Video msg;
		msg._header.setSessionId(_sessionId);
		msg._header.setPktLen(sizeof(msg) + sizeof(sserver::message::BinaryBlock) + videoPac._dataLen);

		if (!sserver::Utils::sendBytes(*_videoSock, &msg, sizeof(msg))) return false;
		sserver::message::BinaryBlock binBlk;
		binBlk.setContentLen(videoPac._dataLen);
		if (!sserver::Utils::sendBytes(*_videoSock, &binBlk, sizeof(binBlk))) return false;
		return sserver::Utils::sendBytes(*_videoSock, videoPac._pData, videoPac._dataLen);
	}

    // ------------------------------------------------------------------------------------------
    bool UserImpl::send_Audio(const User::Audio &audioPac)
    {
        assert(_sessionId != 0);
        assert((_audioSock != NULL) || (audioPac._dataLen != 0) || (audioPac._pData != NULL));

        // don't need lock here

        sserver::message::Audio msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(msg) + sizeof(sserver::message::BinaryBlock) + audioPac._dataLen);

        if (!sserver::Utils::sendBytes(*_audioSock, &msg, sizeof(msg))) return false;
        sserver::message::BinaryBlock binBlk;
        binBlk.setContentLen(audioPac._dataLen);
        if (!sserver::Utils::sendBytes(*_audioSock, &binBlk, sizeof(binBlk))) return false;
        return sserver::Utils::sendBytes(*_audioSock, audioPac._pData, audioPac._dataLen);
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_AddDealOfPrescriptionDrug(const User::DealOfPrescriptionDrug &deal)
    {
		assert(_sessionId != 0);

		sserver::message::AddDealOfPrescriptionDrug msg;
		msg._header.setSessionId(_sessionId);
		msg._header.setPktLen(sizeof(sserver::message::AddDealOfPrescriptionDrug) +
			8 * sizeof(sserver::message::BinaryBlock) +
			deal._buyerName.length() +
			deal._drug_huohao.length() + 
			deal._drug_mingcheng.length() +
			deal._drug_pihao.length() +
	        deal._drug_guige.length() +
            deal._drug_jiliang.length() +
            deal._drug_shengchanchangjia.length() +
            deal._drug_chufanglaiyuan.length());

		msg.setUpdateDealId(deal._updateDealId);
		msg.setBuyerAge(deal._buyerAge);
		msg.setBuyerIsMale(deal._buyerIsMale);

		if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

		sserver::message::BinaryBlock binBlk;
		binBlk.setContentLen(deal._buyerName.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, deal._buyerName.c_str(), deal._buyerName.length())) return false;

		binBlk.setContentLen(deal._drug_huohao.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, deal._drug_huohao.c_str(), deal._drug_huohao.length())) return false;

		binBlk.setContentLen(deal._drug_mingcheng.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, deal._drug_mingcheng.c_str(), deal._drug_mingcheng.length())) return false;

		binBlk.setContentLen(deal._drug_pihao.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, deal._drug_pihao.c_str(), deal._drug_pihao.length())) return false;

		binBlk.setContentLen(deal._drug_guige.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, deal._drug_guige.c_str(), deal._drug_guige.length())) return false;

		binBlk.setContentLen(deal._drug_jiliang.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, deal._drug_jiliang.c_str(), deal._drug_jiliang.length())) return false;

		binBlk.setContentLen(deal._drug_shengchanchangjia.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, deal._drug_shengchanchangjia.c_str(), deal._drug_shengchanchangjia.length())) return false;

		binBlk.setContentLen(deal._drug_chufanglaiyuan.length());
		if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
		if (!sserver::Utils::sendBytes(*_sock, deal._drug_chufanglaiyuan.c_str(), deal._drug_chufanglaiyuan.length())) return false;

   	    return true;
    }

    bool UserImpl::req_AddDealOfPrescriptionDrug(const User::DealOfPrescriptionDrug &deal)
    {
        assert(_sock != NULL);
        if (deal._buyerName.empty() || deal._drug_mingcheng.empty()) return false;

        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_AddDealOfPrescriptionDrug(deal);
    }

    bool UserImpl::req_AddDealOfPrescriptionDrug_Sync(const User::DealOfPrescriptionDrug &deal)
    {
        assert(_sock != NULL);
        if (deal._buyerName.empty() || deal._drug_mingcheng.empty()) return false;

        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_AddDealOfPrescriptionDrug(deal);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_ListDealsOfPrescriptionDrug(unsigned int storeId, 
                                                    const std::string &startTime,
                                                    const std::string &endTime)
    {
        sserver::message::ListDealsOfPrescriptionDrug msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(msg));
        msg.setStoreId(storeId);
        msg.setStartTime(startTime);
        msg.setEndTime(endTime);
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }
    
    bool UserImpl::req_ListDealsOfPrescriptionDrug(unsigned int storeId, 
                                                   const std::string &startTime,
                                                   const std::string &endTime)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_ListDealsOfPrescriptionDrug(storeId, startTime, endTime);
    }

    bool UserImpl::req_ListDealsOfPrescriptionDrug_Sync(unsigned int storeId, 
                                                        const std::string &startTime,
                                                        const std::string &endTime)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_ListDealsOfPrescriptionDrug(storeId, startTime, endTime);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_AddDealOfSpecialDrug(const User::DealOfSpecialDrug &deal)
    {
        assert(_sessionId != 0);

        sserver::message::AddDealOfSpecialDrug msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(sserver::message::AddDealOfSpecialDrug) +
            10 * sizeof(sserver::message::BinaryBlock) +
            deal._buyerName.length() +
            deal._buyerShenFenZheng.length() +
            deal._drug_huohao.length() + 
            deal._drug_mingcheng.length() +
            deal._drug_pihao.length() +
            deal._drug_guige.length() +
            deal._drug_jiliang.length() +
            deal._drug_goumaishuliang.length() + 
            deal._drug_shengchanchangjia.length() +
            deal._drug_chufanglaiyuan.length());

        msg.setUpdateDealId(deal._updateDealId);
        msg.setBuyerAge(deal._buyerAge);
        msg.setBuyerIsMale(deal._buyerIsMale);

        if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

        sserver::message::BinaryBlock binBlk;
        binBlk.setContentLen(deal._buyerName.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._buyerName.c_str(), deal._buyerName.length())) return false;

        binBlk.setContentLen(deal._buyerShenFenZheng.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._buyerShenFenZheng.c_str(), deal._buyerShenFenZheng.length())) return false;

        binBlk.setContentLen(deal._drug_huohao.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._drug_huohao.c_str(), deal._drug_huohao.length())) return false;

        binBlk.setContentLen(deal._drug_mingcheng.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._drug_mingcheng.c_str(), deal._drug_mingcheng.length())) return false;

        binBlk.setContentLen(deal._drug_pihao.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._drug_pihao.c_str(), deal._drug_pihao.length())) return false;

        binBlk.setContentLen(deal._drug_guige.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._drug_guige.c_str(), deal._drug_guige.length())) return false;

        binBlk.setContentLen(deal._drug_jiliang.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._drug_jiliang.c_str(), deal._drug_jiliang.length())) return false;

        binBlk.setContentLen(deal._drug_goumaishuliang.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._drug_goumaishuliang.c_str(), deal._drug_goumaishuliang.length())) return false;

        binBlk.setContentLen(deal._drug_shengchanchangjia.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._drug_shengchanchangjia.c_str(), deal._drug_shengchanchangjia.length())) return false;

        binBlk.setContentLen(deal._drug_chufanglaiyuan.length());
        if (!sserver::Utils::sendBytes(*_sock, &binBlk, sizeof(binBlk))) return false;
        if (!sserver::Utils::sendBytes(*_sock, deal._drug_chufanglaiyuan.c_str(), deal._drug_chufanglaiyuan.length())) return false;

        return true;
    }

    bool UserImpl::req_AddDealOfSpecialDrug(const User::DealOfSpecialDrug &deal)
    {
        assert(_sock != NULL);
        if (deal._buyerShenFenZheng.empty() || deal._drug_mingcheng.empty()) return false;

        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_AddDealOfSpecialDrug(deal);
    }

    bool UserImpl::req_AddDealOfSpecialDrug_Sync(const User::DealOfSpecialDrug &deal)
    {
        assert(_sock != NULL);
        if (deal._buyerShenFenZheng.empty() || deal._drug_mingcheng.empty()) return false;

        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_AddDealOfSpecialDrug(deal);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_ListDealsOfSpecialDrug(unsigned int storeId, 
                                               const std::string &startTime,
                                               const std::string &endTime)
    {
        sserver::message::ListDealsOfSpecialDrug msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(msg));
        msg.setStoreId(storeId);
        msg.setStartTime(startTime);
        msg.setEndTime(endTime);
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

    bool UserImpl::req_ListDealsOfSpecialDrug(unsigned int storeId, 
                                              const std::string &startTime,
                                              const std::string &endTime)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_ListDealsOfSpecialDrug(storeId, startTime, endTime);
    }

    bool UserImpl::req_ListDealsOfSpecialDrug_Sync(unsigned int storeId, 
                                                   const std::string &startTime,
                                                   const std::string &endTime)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_ListDealsOfSpecialDrug(storeId, startTime, endTime);
        _syncEvt.wait();
        return rst;
    }

    // ------------------------------------------------------------------------------------------
    bool UserImpl::_req_ListConsultingDetails(unsigned int storeId, 
                                              const std::string &startTime,
                                              const std::string &endTime)
    {
        sserver::message::ListConsultingDetails msg;
        msg._header.setSessionId(_sessionId);
        msg._header.setPktLen(sizeof(msg));
        msg.setStoreId(storeId);
        msg.setStartTime(startTime);
        msg.setEndTime(endTime);
        return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
    }

    bool UserImpl::req_ListConsultingDetails(unsigned int storeId, 
                                             const std::string &startTime,
                                             const std::string &endTime)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);
        return _req_ListConsultingDetails(storeId, startTime, endTime);
    }

    bool UserImpl::req_ListConsultingDetails_Sync(unsigned int storeId, 
                                                  const std::string &startTime,
                                                  const std::string &endTime)
    {
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        _syncEvt.reset();
        bool rst = _req_ListConsultingDetails(storeId, startTime, endTime);
        _syncEvt.wait();
        return rst;
    }

	// ------------------------------------------------------------------------------------------
	bool UserImpl::_req_ListPharmacistActivity(unsigned int pharmacistId, 
		                                       const std::string &startTime,
		                                       const std::string &endTime)
	{
		sserver::message::ListPharmacistActivity msg;
		msg._header.setSessionId(_sessionId);
		msg._header.setPktLen(sizeof(msg));
		msg.setPharmacistId(pharmacistId);
		msg.setStartTime(startTime);
		msg.setEndTime(endTime);
		return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
	}
	
	bool UserImpl::req_ListPharmacistActivity(unsigned int pharmacistId, 
		                                      const std::string &startTime,
		                                      const std::string &endTime)
	{
		assert(_sock != NULL);
		Poco::FastMutex::ScopedLock lock(_sendMutex);
		return _req_ListPharmacistActivity(pharmacistId, startTime, endTime);
	}

	bool UserImpl::req_ListPharmacistActivity_Sync(unsigned int pharmacistId, 
		                                           const std::string &startTime,
		                                           const std::string &endTime)
	{
		assert(_sock != NULL);
		Poco::FastMutex::ScopedLock lock(_sendMutex);

		_syncEvt.reset();
		bool rst = _req_ListPharmacistActivity(pharmacistId, startTime, endTime);
		_syncEvt.wait();
		return rst;
	}

    // ------------------------------------------------------------------------------------------
    bool UserImpl::stopConverstaion()
    {
        assert(_sessionId != 0);
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        sserver::message::StopConversation msg;
        msg._header.setPktLen(sizeof(msg));
        msg._header.setSessionId(_sessionId);
        if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

        _isReceivingVideo = false;
        _isReceivingAudio = false;       
        
		// only shutdown the video and audio sockets
		sserver::Utils::closeSocket(*_videoSock);
		sserver::Utils::closeSocket(*_audioSock);

        return true;
    }

	// ------------------------------------------------------------------------------------------
	bool UserImpl::send_HB()
	{
		assert(_sessionId != 0);
		Poco::FastMutex::ScopedLock lock(_sendMutex);

		sserver::message::ClientHeartbeat msg;
		msg._header.setSessionId(_sessionId);
		msg._header.setPktLen(sizeof(msg));

		return sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg));
	}

    // ------------------------------------------------------------------------------------------
    bool UserImpl::logout()
    {
        assert(_sessionId != 0);
        assert(_sock != NULL);
        Poco::FastMutex::ScopedLock lock(_sendMutex);

        sserver::message::LogoutRequest msg;
        msg._header.setPktLen(sizeof(msg));
        msg._header.setSessionId(_sessionId);
        if (!sserver::Utils::sendBytes(*_sock, &msg, sizeof(msg))) return false;

        _isReceiving = false;
		sserver::Utils::closeSocket(*_sock);

        return true;
    }

} // namespace client
