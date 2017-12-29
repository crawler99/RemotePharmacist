#pragma once

#include "Poco/Thread.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"
#include "tbb/atomic.h"
#include "DatabaseManager.h"
#include <map>
#include "../message/Enums.h"

namespace sserver { namespace server {

    class RelaxedSocketReactor : public Poco::Net::SocketReactor
    {
        virtual void onIdle()
        {
            Poco::Thread::sleep(1);
        }

        virtual void onTimeout()
        {
            Poco::Thread::sleep(1);
        }
    };

    class WorkerManager;

    class Worker
    {
    public:
        Worker(Poco::SharedPtr<DatabaseManager> pDBM, WorkerManager *pWM);
		~Worker();

        void start();
        void stop();
        void assignClient(Poco::Net::StreamSocket sock);

		// this is a special message handler made public for outside usage
		void onLogoutRequest(Poco::Net::StreamSocket &sock, char *pMsg);

    private:
        void _cleanupSock(Poco::Net::StreamSocket &sock);

        void _onSockReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification> &pNotification);
        void _onSockError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNotification);

        // packet processor functor
        typedef void(Worker::*PMF)(Poco::Net::StreamSocket &sock, char*);

        void _registerFunctor(sserver::message::PKT_TYPE pktType, PMF pmf)
        {
            _functors[pktType] = pmf;
        }

        void _onLoginRequest(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onRefreshFingerprint(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onAddPharmacistCertType(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onListPharmacistCertTypes(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onAddStore(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onDeleteStores(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onListStores(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onPharmacistAdd(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onDeletePharmacists(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onPharmacistsBriefList(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onPharmacistsDetailList(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onPatientAdd(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onGetPharmacistsDetailOfCurStore(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onPharmacistRequest(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onPrescription(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onVideo(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onAudio(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onAddDealOfPrescriptionDrug(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onListDealsOfPrescriptionDrug(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onAddDealOfSpecialDrug(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onListDealsOfSpecialDrug(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onListConsultingDetails(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onListPharmacistActivity(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onClientHeartbeat(Poco::Net::StreamSocket &sock, char *pMsg);
		void _onStopConversationRequest(Poco::Net::StreamSocket &sock, char *pMsg);
        void _onShutDownRequest(Poco::Net::StreamSocket &sock, char *pMsg);
	
		// this is called on socket error
		void _onFakeLogout(Poco::Net::StreamSocket &sock, Poco::UInt32 sessionId);

        // references to manager objects
        Poco::SharedPtr<DatabaseManager> _pDBM;
        WorkerManager *_pWM;  // hold a raw pointer here for compilation

        // socket reactor and its thread
        RelaxedSocketReactor _sockReactor;
        Poco::Thread _sThread;

        // control flag
        tbb::atomic<bool> _stop;

        // receiving buffer
        char *_pRecvBuff;

        // packet processor functor map
        std::map<sserver::message::PKT_TYPE, PMF> _functors;
    };
} }