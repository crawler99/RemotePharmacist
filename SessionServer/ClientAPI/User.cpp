#include "User.h"
#include "UserImpl.h"

namespace client {

    User::User(const std::string &username, 
               const std::string &password,
               UserCallback *userCallback)
    {
        _pImpl = new UserImpl(username, password, userCallback);
    }

    bool User::connect(const std::string &serverIP, unsigned int serverPort)
    {
        return _pImpl->connect(serverIP, serverPort);
    }

    void User::startReceiving()
    {
        _pImpl->startReceiving(*this);
    }

	void User::startReceivingVideo()
	{
		_pImpl->startReceivingVideo(*this);
	}

	void User::startReceivingAudio()
	{
		_pImpl->startReceivingAudio(*this);
	}

    void User::stopReceiving()
    {
        _pImpl->stopReceiving();
    }

	void User::stopReceivingVideo()
	{
		_pImpl->stopReceivingVideo();
	}

	void User::stopReceivingAudio()
	{
		_pImpl->stopReceivingAudio();
	}

    bool User::req_Login(const char *pFingerprint, unsigned int fpLen)
    {
        return _pImpl->req_Login(pFingerprint, fpLen);
    }

    bool User::req_Login_Sync(const char *pFingerprint, unsigned int fpLen)
    {
        return _pImpl->req_Login_Sync(pFingerprint, fpLen);
    }

	bool User::send_RefreshFingerprint()
	{
		return _pImpl->send_RefreshFingerprint();
	}

    bool User::req_AddPharmacistCertType(const std::string &certTypeName)
    {
        return _pImpl->req_AddPharmacistCertType(certTypeName);
    }

    bool User::req_AddPharmacistCertType_Sync(const std::string &certTypeName)
    {
        return _pImpl->req_AddPharmacistCertType_Sync(certTypeName);
    }

    bool User::req_ListPharmacistCertTypes()
    {
        return _pImpl->req_ListPharmacistCertTypes();
    }

    bool User::req_ListPharmacistCertTypes_Sync()
    {
        return _pImpl->req_ListPharmacistCertTypes_Sync();
    }

    bool User::req_AddStore(const StoreRegInfo &info)
    {
        return _pImpl->req_AddStore(info);
    }

    bool User::req_AddStore_Sync(const StoreRegInfo &info)
    {
        return _pImpl->req_AddStore_Sync(info);
    }

	bool User::req_DeleteStores(const std::set<unsigned int> &storeIds)
	{
		return _pImpl->req_DeleteStores(storeIds);
	}

	bool User::req_DeleteStores_Sync(const std::set<unsigned int> &storeIds)
	{
		return _pImpl->req_DeleteStores_Sync(storeIds);
	}

    bool User::req_ListStores()
    {
        return _pImpl->req_ListStores();
    }

    bool User::req_ListStores_Sync()
    {
        return _pImpl->req_ListStores_Sync();
    }

    bool User::req_AddPharmacist(const PharmacistRegInfo &info)
    {
        return _pImpl->req_AddPharmacist(info);
    }

    bool User::req_AddPharmacist_Sync(const PharmacistRegInfo &info)
    {
        return _pImpl->req_AddPharmacist_Sync(info);
    }

    bool User::req_DeletePharmacists(const std::set<unsigned int>  &pharmacistIds)
    {
        return _pImpl->req_DeletePharmacists(pharmacistIds);
    }

    bool User::req_DeletePharmacists_Sync(const std::set<unsigned int>  &pharmacistIds)
    {
        return _pImpl->req_DeletePharmacists_Sync(pharmacistIds);
    }

    bool User::req_ListPharmacists(bool includePhoto)
    {
        return _pImpl->req_ListPharmacists(includePhoto);
    }

    bool User::req_ListPharmacists_Sync(bool includePhoto)
    {
        return _pImpl->req_ListPharmacists_Sync(includePhoto);
    }

    bool User::req_ListPharmacistsDetail(unsigned int userId)
    {
        return _pImpl->req_ListPharmacistsDetail(userId);
    }

    bool User::req_ListPharmacistsDetail_Sync(unsigned int userId)
    {
        return _pImpl->req_ListPharmacistsDetail_Sync(userId);
    }

    bool User::req_AddStoreAccount(const StoreAccountRegInfo &info)
    {
        return _pImpl->req_AddStoreAccount(info);
    }

    bool User::req_AddStoreAccount_Sync(const StoreAccountRegInfo &info)
    {
        return _pImpl->req_AddStoreAccount_Sync(info);
    }

    bool User::req_PharmacistsDetailOfCurStore()
    {
        return _pImpl->req_PharmacistsDetailOfCurStore();
    }

    bool User::req_PharmacistsDetailOfCurStore_Sync()
    {
        return _pImpl->req_PharmacistsDetailOfCurStore_Sync();
    }

    bool User::req_Pharmacist(unsigned int certTypeId, bool usePool)
    {
        return _pImpl->req_Pharmacist(certTypeId, usePool);
    }

    bool User::req_Pharmacist_Sync(unsigned int certTypeId, bool usePool)
    {
        return _pImpl->req_Pharmacist_Sync(certTypeId, usePool);
    }

    bool User::send_Prescription(const Prescription &prescription)
    {
        return _pImpl->send_Prescription(prescription);
    }

	bool User::send_Video(const Video &videoPac)
	{
		return _pImpl->send_Video(videoPac);
	}

    bool User::send_Audio(const Audio &audioPac)
    {
        return _pImpl->send_Audio(audioPac);
    }

    bool User::req_AddDealOfPrescriptionDrug(const DealOfPrescriptionDrug &deal)
    {
        return _pImpl->req_AddDealOfPrescriptionDrug(deal);
    }

    bool User::req_AddDealOfPrescriptionDrug_Sync(const DealOfPrescriptionDrug &deal)
    {
        return _pImpl->req_AddDealOfPrescriptionDrug_Sync(deal);
    }

    bool User::req_ListDealsOfPrescriptionDrug(unsigned int storeId, 
                                               const std::string &startTime,
                                               const std::string &endTime)
    {
        return _pImpl->req_ListDealsOfPrescriptionDrug(storeId, startTime, endTime);
    }

    bool User::req_ListDealsOfPrescriptionDrug_Sync(unsigned int storeId, 
                                                    const std::string &startTime,
                                                    const std::string &endTime)
    {
        return _pImpl->req_ListDealsOfPrescriptionDrug_Sync(storeId, startTime, endTime);
    }

    bool User::req_AddDealOfSpecialDrug(const DealOfSpecialDrug &deal)
    {
        return _pImpl->req_AddDealOfSpecialDrug(deal);
    }

    bool User::req_AddDealOfSpecialDrug_Sync(const DealOfSpecialDrug &deal)
    {
        return _pImpl->req_AddDealOfSpecialDrug_Sync(deal);
    }

    bool User::req_ListDealsOfSpecialDrug(unsigned int storeId, 
                                          const std::string &startTime,
                                          const std::string &endTime)
    {
        return _pImpl->req_ListDealsOfSpecialDrug(storeId, startTime, endTime);
    }

    bool User::req_ListDealsOfSpecialDrug_Sync(unsigned int storeId, 
                                               const std::string &startTime,
                                               const std::string &endTime)
    {
        return _pImpl->req_ListDealsOfSpecialDrug_Sync(storeId, startTime, endTime);
    }

    bool User::req_ListConsultingDetails(unsigned int storeId, 
                                         const std::string &startTime,
                                         const std::string &endTime)
    {
        return _pImpl->req_ListConsultingDetails(storeId, startTime, endTime);
    }

    bool User::req_ListConsultingDetails_Sync(unsigned int storeId, 
                                              const std::string &startTime,
                                              const std::string &endTime)
    {
        return _pImpl->req_ListConsultingDetails_Sync(storeId, startTime, endTime);
    }

	bool User::req_ListPharmacistActivity(unsigned int pharmacistId, 
		                                  const std::string &startTime,
		                                  const std::string &endTime)
	{
		return _pImpl->req_ListPharmacistActivity(pharmacistId, startTime, endTime);
	}

	bool User::req_ListPharmacistActivity_Sync(unsigned int pharmacistId, 
		                                       const std::string &startTime,
		                                       const std::string &endTime)
	{
		return _pImpl->req_ListPharmacistActivity_Sync(pharmacistId, startTime, endTime);
	}

    bool User::stopConverstaion()
    {
        return _pImpl->stopConverstaion();
    }

	bool User::send_HB()
	{
		return _pImpl->send_HB();
	}

    bool User::logout()
    {
        return _pImpl->logout();
    }

} // namespace client