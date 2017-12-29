#pragma once

#include "Poco/Util/XMLConfiguration.h"
#include "Poco/SharedPtr.h"
#include "Poco/RWLock.h"
#include "Poco/Timer.h"
#include "DatabaseManager.h"
#include "Worker.h"
#include "SimpleFileSaver.h"
#include "ContinuousFileSaver.h"
#include <queue>

namespace sserver { namespace server {

	class PatientConn
	{
	public:
		PatientConn() {}
		PatientConn(Poco::UInt32 sessionId, Poco::UInt32 userId, Poco::Net::StreamSocket sock, Poco::UInt32 storeId)
			: _sessionId(sessionId), _userId(userId), _sock(sock), _storeId(storeId) {}

		Poco::UInt32             _sessionId;
		Poco::UInt32             _userId;
		Poco::Net::StreamSocket  _sock;
		Poco::UInt32             _storeId;
	};

	class PharmacistConn
	{
	public:
		PharmacistConn() {}
		PharmacistConn(Poco::UInt32 sessionId, Poco::UInt32 userId, 
                       Poco::UInt32 certTypeId, Poco::Net::StreamSocket sock)
			: _sessionId(sessionId), _userId(userId), _certTypeId(certTypeId), _sock(sock) {}

		Poco::UInt32             _sessionId;
		Poco::UInt32             _userId;
        Poco::UInt32             _certTypeId;
		Poco::Net::StreamSocket  _sock;
	};

    struct ConversationEnd_Pharmacist
    {
        PharmacistConn  _conn;
        Poco::UInt32    _conversationId;
    };

    struct ConversationEnd_Patient
    {
        PatientConn     _conn;
        Poco::UInt32    _conversationId;
    };

	struct ClientHBExaminee
	{
		Poco::Net::StreamSocket _sock;
		Worker                  *_worker;
		Poco::Timestamp         _timestamp;
	};
    
    class WorkerManager
    {
    public:
        WorkerManager(Poco::SharedPtr<DatabaseManager> pDBM);
        virtual ~WorkerManager();

        virtual bool init(Poco::Util::XMLConfiguration *pConfig);
        virtual void assignClient(Poco::Net::StreamSocket sock);

		// ---------------------------------------------------------------------------------------------------------------------
		void onPharmacistIn(Poco::UInt32 sessionId, const PharmacistConn &pharmacistConn);
		void onPatientIn(Poco::UInt32 sessionId, const PatientConn &patientConn);

		PharmacistConn onRequestForPharmacist(Poco::Net::StreamSocket sock, Poco::UInt32 sessionId, 
			                                  const std::set<Poco::UInt32> &pharmacistIds);
        PharmacistConn onRequestForPharmacist_UsePool(Poco::Net::StreamSocket sock, Poco::UInt32 sessionId,
			                                          Poco::UInt32 certTypeId);
		void fillConversationId(Poco::UInt32 conversationId, Poco::UInt32 pharmacistSessionId, Poco::UInt32 patientSessionId);

		bool onPatientQuitConversation(Poco::UInt32 patientSessionId, PharmacistConn &targetPharmacist);
		bool onPharmacistQuitConversation(Poco::UInt32 pharmacistSessionId, PatientConn &targetPatient);

		void onPharmacistOut(Poco::UInt32 sessionId);
		void onPatientOut(Poco::UInt32 sessionId);

		void onClientHeartbeat(Poco::UInt32 sessionId, const ClientHBExaminee &examinee);

		// ---------------------------------------------------------------------------------------------------------------------

		// look up the current connection
        const PharmacistConn* getPharmacistConn(Poco::UInt32 pharmacistSessionId);
        const PatientConn* getPatientConn(Poco::UInt32 patientSessionId);
		Poco::UInt32 getSessionId(const Poco::Net::StreamSocket &sock);
        
        // look up the other side of conversation
        ConversationEnd_Pharmacist* getTargetPharmacist(Poco::UInt32 patientSessionId);
        ConversationEnd_Patient* getTargetPatient(Poco::UInt32 pharmacistSessionId);
        Poco::Net::StreamSocket* getTargetPharmacistVideoSock(Poco::UInt32 patientSessionId, 
                                                              const Poco::Net::StreamSocket &patientVideoSock);
        Poco::Net::StreamSocket* getTargetPatientVideoSock(Poco::UInt32 pharmacistSessionId, 
                                                           const Poco::Net::StreamSocket &pharmacistVideoSock);
        Poco::Net::StreamSocket* getTargetPharmacistAudioSock(Poco::UInt32 patientSessionId, 
                                                              const Poco::Net::StreamSocket &patientAudioSock);
        Poco::Net::StreamSocket* getTargetPatientAudioSock(Poco::UInt32 pharmacistSessionId, 
                                                           const Poco::Net::StreamSocket &pharmacistAudioSock);

		// expose prescription saver
		Poco::SharedPtr<SimpleFileSaver> getPrescriptionSaver() { return _pPrescriptionSaver; }
        // expose video saver
        Poco::SharedPtr<ContinuousFileSaver> getVideoSaver() { return _pVideoSaver; }
		// expose audio saver
		Poco::SharedPtr<ContinuousFileSaver> getAudioSaver() { return _pAudioSaver; }

    private:
        // use the global XML config file
        Poco::Util::XMLConfiguration *_pConfig;

        // refer to database manager
        Poco::SharedPtr<DatabaseManager> _pDBM;

        // worker pool
        unsigned int _workerNum;
        tbb::atomic<unsigned int> _nextWorkerIdx;
        std::vector<Poco::SharedPtr<Worker> > _workerList;

        // ====== Main socket channel ====================

        // <sessionId, pharmacist conn>
        std::map<Poco::UInt32, PharmacistConn> _pharmacistConns;
        // <sessionId, patient conn>
        std::map<Poco::UInt32, PatientConn> _patientConns;
        // <pharmacistId, PharmacistConn> 
        std::map<Poco::UInt32, PharmacistConn> _idlePharmacists;

		// <sock, sessionId>
		std::map<Poco::Net::StreamSocket, Poco::UInt32> _socks2Session;

        // <patient sessionId, ... >
		std::map<Poco::UInt32, ConversationEnd_Pharmacist> _patientToPharmacist;
		// <pharmacist sessionId, ... >
		std::map<Poco::UInt32, ConversationEnd_Patient> _pharmacistToPatient;

        // ====== Video socket channel ====================

        // <patient sessionId, pharmacist video socket>
		std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> > _patientToPharmacistVideo;
        // <pharmacist sessionId, patient video socket>
		std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> > _pharmacistToPatientVideo;

        // ====== Audio socket channel ====================

        // <patient sessionId, pharmacist audio socket>
        std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> > _patientToPharmacistAudio;
        // <pharmacist sessionId, patient video socket>
        std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> > _pharmacistToPatientAudio;

        // =================================================

        unsigned int _numOfIdlePharmacists;
        unsigned int _numOfBusyPharmacists;

		Poco::RWLock _lock;          // main lock
        Poco::RWLock _videoLock;     // video lock
        Poco::RWLock _audioLock;     // audio lock

		// prescription saver
		std::string                          _prescriptionDir;
		Poco::SharedPtr<SimpleFileSaver>     _pPrescriptionSaver;

        // video saver
        std::string                          _videoDir;
        Poco::SharedPtr<ContinuousFileSaver> _pVideoSaver;

		// audio saver
		std::string                          _audioDir;
		Poco::SharedPtr<ContinuousFileSaver> _pAudioSaver;

		// client heartbeat handler
		unsigned int                             _clientHBTimeoutSec;
		std::map<Poco::UInt32, ClientHBExaminee> _clientHBExaminees;
		Poco::Timer                              _clientHBTimer;
		bool                                     _checkPharmacistHB;
		bool                                     _checkPatientHB;
		void _onClientHBTimer(Poco::Timer &timer);
    };
    
} }