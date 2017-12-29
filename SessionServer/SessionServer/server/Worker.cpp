#include "Worker.h"
#include "WorkerManager.h"
#include "../message/CommonMessages.h"
#include "../logger/Logger.h"
#include "../util/Utils.h"
#include "Poco/NObserver.h"
#include "Poco/Net/NetException.h"

namespace sserver { namespace server {

    Worker::Worker(Poco::SharedPtr<DatabaseManager> pDBM, WorkerManager *pWM)
    {
        _pDBM = pDBM;
        _pWM = pWM;
        _stop = false;

		_pRecvBuff = new char[1024*1024*4];

        // register packet processor functors
        _registerFunctor(sserver::message::PKT_TYPE_LOGIN, &Worker::_onLoginRequest);
		_registerFunctor(sserver::message::PKT_TYPE_REFRESH_FINGERPRINT, &Worker::_onRefreshFingerprint);
        _registerFunctor(sserver::message::PKT_TYPE_ADD_PHARMACIST_CERT_TYPE, &Worker::_onAddPharmacistCertType);
        _registerFunctor(sserver::message::PKT_TYPE_LIST_PHARMACIST_CERT_TYPES, &Worker::_onListPharmacistCertTypes);
		_registerFunctor(sserver::message::PKT_TYPE_ADD_STORE, &Worker::_onAddStore);
		_registerFunctor(sserver::message::PKT_TYPE_DELETE_STORES, &Worker::_onDeleteStores);
        _registerFunctor(sserver::message::PKT_TYPE_LIST_STORES, &Worker::_onListStores);
		_registerFunctor(sserver::message::PKT_TYPE_PHARMACIST_ADD, &Worker::_onPharmacistAdd);
        _registerFunctor(sserver::message::PKT_TYPE_DELETE_PHARMACISTS, &Worker::_onDeletePharmacists);
		_registerFunctor(sserver::message::PKT_TYPE_PHARMACISTS_BRIEF_LIST, &Worker::_onPharmacistsBriefList);
        _registerFunctor(sserver::message::PKT_TYPE_PHARMACISTS_DETAIL_LIST, &Worker::_onPharmacistsDetailList);
		_registerFunctor(sserver::message::PKT_TYPE_PATIENT_ADD, &Worker::_onPatientAdd);
        _registerFunctor(sserver::message::PKT_TYPE_GET_PHARMACISTS_DETAIL_OF_CUR_STORE, &Worker::_onGetPharmacistsDetailOfCurStore);
		_registerFunctor(sserver::message::PKT_TYPE_PHARMACIST_REQUEST, &Worker::_onPharmacistRequest);
        _registerFunctor(sserver::message::PKT_TYPE_DATA_TRANSFER_PRESCRIPTION, &Worker::_onPrescription);
        _registerFunctor(sserver::message::PKT_TYPE_DATA_TRANSFER_VIDEO, &Worker::_onVideo);
        _registerFunctor(sserver::message::PKT_TYPE_DATA_TRANSFER_AUDIO, &Worker::_onAudio);
		_registerFunctor(sserver::message::PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG, &Worker::_onAddDealOfPrescriptionDrug);
        _registerFunctor(sserver::message::PKT_TYPE_LIST_DEALS_OF_PRESCRIPTION_DRUG, &Worker::_onListDealsOfPrescriptionDrug);
        _registerFunctor(sserver::message::PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG, &Worker::_onAddDealOfSpecialDrug);
        _registerFunctor(sserver::message::PKT_TYPE_LIST_DEALS_OF_SPECIAL_DRUG, &Worker::_onListDealsOfSpecialDrug);
        _registerFunctor(sserver::message::PKT_TYPE_LIST_CONSULTING_DETAILS, &Worker::_onListConsultingDetails);
		_registerFunctor(sserver::message::PKT_TYPE_LIST_PHARMACIST_ACTIVITY, &Worker::_onListPharmacistActivity);
		_registerFunctor(sserver::message::PKT_TYPE_CLIENT_HEARTBEAT, &Worker::_onClientHeartbeat);
		_registerFunctor(sserver::message::PKT_TYPE_STOP_CONVERSATION, &Worker::_onStopConversationRequest);
		_registerFunctor(sserver::message::PKT_TYPE_LOGOUT, &Worker::onLogoutRequest);
        _registerFunctor(sserver::message::PKT_TYPE_SHUTDOWN_SERVER, &Worker::_onShutDownRequest);
    }

	Worker::~Worker()
	{
		delete [] _pRecvBuff;
	}

    void Worker::assignClient(Poco::Net::StreamSocket sock)
    {
        if (!_stop)
        {
            _sockReactor.addEventHandler
                (sock, Poco::NObserver<Worker, Poco::Net::ReadableNotification>(*this, &Worker::_onSockReadable));
            _sockReactor.addEventHandler
                (sock, Poco::NObserver<Worker, Poco::Net::ErrorNotification>(*this, &Worker::_onSockError));
			LOG_INFO("Client socket accepted: " + sock.peerAddress().toString());                
        }
        else
        {
            LOG_INFO("Worker is stopped, discard client socket");
            _cleanupSock(sock);
        }
    }

    void Worker::start()
    {
        LOG_INFO("Starting worker ...");
        _sThread.start(_sockReactor);
        LOG_INFO("Worker is started");
    }

    void Worker::stop()
    {
        LOG_INFO("Stopping worker ...");
        _stop = true;
        _sockReactor.stop();
        _sThread.join();
        LOG_INFO("Worker stopped");
    }

    void Worker::_onSockReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification> &pNotification)
    {           
        Poco::AutoPtr<Poco::Net::ReadableNotification> &pNotif = 
            const_cast<Poco::AutoPtr<Poco::Net::ReadableNotification>&> (pNotification);
        Poco::Net::StreamSocket sock(pNotif->socket());

        // read packet header
        unsigned int headerSize = sizeof(message::Header);
        if (!Utils::recvBytes(sock, _pRecvBuff, headerSize)) 
        { 
            LOG_ERROR("Socket receiving error, shutting down and close ...");
            _cleanupSock(sock);
            return;
        }

        // read message body and pass the whole packet to processor function
        message::Header *pHeader = reinterpret_cast<message::Header*>(_pRecvBuff);
        PMF pmf = _functors[static_cast<sserver::message::PKT_TYPE>(pHeader->getPktType())];
        if (pmf)
        {
            if (!Utils::recvBytes(sock, _pRecvBuff + headerSize, pHeader->getPktLen() - headerSize)) 
            { 
                LOG_ERROR("Socket receiving error, shutting down and close ...");
                _cleanupSock(sock);
                return;
            }
            (this->*pmf)(sock, _pRecvBuff);
        }
        else
        {
            std::string pktTypeStr;
            Utils::num2String<Poco::UInt16>(pHeader->getPktType(), pktTypeStr);
            LOG_ERROR(std::string("Received unrecognized message type: ") + pktTypeStr);
        }
    }

    void Worker::_onSockError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNotification)
    {
        Poco::AutoPtr<Poco::Net::ErrorNotification> &pNotif = 
            const_cast<Poco::AutoPtr<Poco::Net::ErrorNotification>&> (pNotification);
        Poco::Net::StreamSocket sock(pNotif->socket());

        std::string errMsg("Client socket error [");
        LOG_ERROR(errMsg + "peer = " + pNotif->socket().peerAddress().toString() + "], shutting down and close ...");
        _cleanupSock(sock);
    }

    void Worker::_cleanupSock(Poco::Net::StreamSocket &sock)
    {
        _sockReactor.removeEventHandler(sock, Poco::NObserver<Worker, Poco::Net::ReadableNotification>(*this, &Worker::_onSockReadable));
        _sockReactor.removeEventHandler(sock, Poco::NObserver<Worker, Poco::Net::ErrorNotification>(*this, &Worker::_onSockError));
        
		Poco::UInt32 sessionId = _pWM->getSessionId(sock);
		if (sessionId != 0)
		{
			_onFakeLogout(sock, sessionId);
		}
		
		Utils::closeSocket(sock);
    }
    
} }