#pragma once

#include "User.h"
#include "Poco/Mutex.h"
#include "Poco/Event.h"
#include "tbb/atomic.h"
#include "../SessionServer/message/CommonMessages.h"

namespace Poco { namespace Net { class StreamSocket; } }

namespace client {

    class UserImpl
    {
    public:
        UserImpl(const std::string &username, 
                 const std::string &password,
                 UserCallback *userCallBack);

		bool connect(const std::string &serverIP, unsigned int serverPort);

        void startReceiving(User &user);
		void startReceivingVideo(User &user);
		void startReceivingAudio(User &user);

        void stopReceiving();
		void stopReceivingVideo();
		void stopReceivingAudio();

        // 
        bool req_Login(const char *pFingerprint, unsigned int fpLen);
        bool req_Login_Sync(const char *pFingerprint, unsigned int fpLen);
		bool send_RefreshFingerprint();

        //
        bool req_AddPharmacistCertType(const std::string &certTypeName);
        bool req_AddPharmacistCertType_Sync(const std::string &certTypeName);

        //
        bool req_ListPharmacistCertTypes();
        bool req_ListPharmacistCertTypes_Sync();

        //
        bool req_AddPharmacist(const User::PharmacistRegInfo &info);
        bool req_AddPharmacist_Sync(const User::PharmacistRegInfo &info);
        bool req_DeletePharmacists(const std::set<unsigned int> &pharmacistIds);
        bool req_DeletePharmacists_Sync(const std::set<unsigned int> &pharmacistIds);

        //
        bool req_ListPharmacists(bool includePhoto);
        bool req_ListPharmacists_Sync(bool includePhoto);

        //
        bool req_ListPharmacistsDetail(unsigned int userId);
        bool req_ListPharmacistsDetail_Sync(unsigned int userId);

        //
        bool req_AddStore(const User::StoreRegInfo &info);
        bool req_AddStore_Sync(const User::StoreRegInfo &info);
		bool req_DeleteStores(const std::set<unsigned int> &storeIds);
		bool req_DeleteStores_Sync(const std::set<unsigned int> &storeIds);

        //
        bool req_ListStores();
        bool req_ListStores_Sync();

        //
        bool req_AddStoreAccount(const User::StoreAccountRegInfo &info);
        bool req_AddStoreAccount_Sync(const User::StoreAccountRegInfo &info);

        //
        bool req_PharmacistsDetailOfCurStore();
        bool req_PharmacistsDetailOfCurStore_Sync();

        //
		bool req_Pharmacist(unsigned int certTypeId, bool usePool);
        bool req_Pharmacist_Sync(unsigned int certTypeId, bool usePool);

        //
        bool send_Prescription(const User::Prescription &prescription);

		//
		bool send_Video(const User::Video &videoPac);

        //
        bool send_Audio(const User::Audio &audioPac);

        //
        bool req_AddDealOfPrescriptionDrug(const User::DealOfPrescriptionDrug &deal);
        bool req_AddDealOfPrescriptionDrug_Sync(const User::DealOfPrescriptionDrug &deal);

        // 
        bool req_ListDealsOfPrescriptionDrug(unsigned int storeId, 
                                             const std::string &startTime,
                                             const std::string &endTime);
        bool req_ListDealsOfPrescriptionDrug_Sync(unsigned int storeId, 
                                                  const std::string &startTime,
                                                  const std::string &endTime);

        // 
        bool req_AddDealOfSpecialDrug(const User::DealOfSpecialDrug &deal);
        bool req_AddDealOfSpecialDrug_Sync(const User::DealOfSpecialDrug &deal);

        // 
        bool req_ListDealsOfSpecialDrug(unsigned int storeId, 
                                        const std::string &startTime,
                                        const std::string &endTime);
        bool req_ListDealsOfSpecialDrug_Sync(unsigned int storeId, 
                                             const std::string &startTime,
                                             const std::string &endTime);

        // 
        bool req_ListConsultingDetails(unsigned int storeId, 
                                       const std::string &startTime,
                                       const std::string &endTime);
        bool req_ListConsultingDetails_Sync(unsigned int storeId, 
                                            const std::string &startTime,
                                            const std::string &endTime);

		//
		bool req_ListPharmacistActivity(unsigned int pharmacistId, 
			                            const std::string &startTime,
			                            const std::string &endTime);
		bool req_ListPharmacistActivity_Sync(unsigned int pharmacistId, 
			                                 const std::string &startTime,
			                                 const std::string &endTime);

        //
        bool stopConverstaion();

		//
		bool send_HB();

        //
        bool logout();

    private:
        bool _req_Login(const char *pFingerprint, unsigned int fpLen);
        bool _req_AddPharmacistCertType(const std::string &certTypeName);
        bool _req_ListPharmacistCertTypes();
        bool _req_ListPharmacists(bool includePhoto);
        bool _req_ListPharmacistsDetail(unsigned int userId);
        bool _req_AddPharmacist(const User::PharmacistRegInfo &info);
        bool _req_DeletePharmacists(const std::set<unsigned int> &pharmacistIds);
        bool _req_AddStore(const User::StoreRegInfo &info);
		bool _req_DeleteStores(const std::set<unsigned int> &storeIds);
        bool _req_ListStores();
        bool _req_AddStoreAccount(const User::StoreAccountRegInfo &info);
        bool _req_PharmacistsDetailOfCurStore();
        bool _req_Pharmacist(unsigned int certTypeId, bool usePool);
        bool _req_AddDealOfPrescriptionDrug(const User::DealOfPrescriptionDrug &deal);
        bool _req_ListDealsOfPrescriptionDrug(unsigned int storeId, 
                                              const std::string &startTime,
                                              const std::string &endTime);
        bool _req_AddDealOfSpecialDrug(const User::DealOfSpecialDrug &deal);
        bool _req_ListDealsOfSpecialDrug(unsigned int storeId, 
                                         const std::string &startTime,
                                         const std::string &endTime);
        bool _req_ListConsultingDetails(unsigned int storeId, 
                                        const std::string &startTime,
                                        const std::string &endTime);
		bool _req_ListPharmacistActivity(unsigned int pharmacistId, 
			                             const std::string &startTime,
			                             const std::string &endTime);

        std::string               _serverIP;
		Poco::UInt16              _serverPort;
		
		std::string               _username;
        std::string               _password;
        UserCallback              *_userCallback;

        Poco::UInt32              _sessionId;

		// ====== Main socket channel ====================
        Poco::Net::StreamSocket   *_sock;
        Poco::FastMutex           _sendMutex;
        Poco::Event               _syncEvt;
        tbb::atomic<bool>         _isReceiving;
        char                      _recvBuff[sizeof(sserver::message::Header)];

		// ====== Video socket channel ====================
		Poco::Net::StreamSocket   *_videoSock;
		tbb::atomic<bool>         _isReceivingVideo;
		char                      _videoRecvBuff[sizeof(sserver::message::Header)];

		// ====== Audio socket channel ====================
		Poco::Net::StreamSocket   *_audioSock;
		tbb::atomic<bool>         _isReceivingAudio;
		char                      _audioRecvBuff[sizeof(sserver::message::Header)];
    };

} // namespace client