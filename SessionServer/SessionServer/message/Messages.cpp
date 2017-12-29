#include "Enums.h"
#include "CommonMessages.h"
#include "ClientMessages.h"
#include "ServerMessages.h"
#include "../util//Utils.h"

namespace sserver { namespace message {

    // ===================================================================================
    Header::Header()
    {
    }

    Poco::UInt32 Header::getPktLen() const
    {
        return Utils::readNumericField<Poco::UInt32>(_pktLen);
    }

    Poco::UInt16 Header::getPktVersion() const
    {
        return Utils::readNumericField<Poco::UInt16>(_pktVersion);
    }

    Poco::UInt16 Header::getPktType() const
    {
        return Utils::readNumericField<Poco::UInt16>(_pktType);
    }

    Poco::UInt32 Header::getSessionId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_sessionId);
    }

    void Header::setPktLen(Poco::UInt32 pktLen)
    {
        Utils::writeNumericField<Poco::UInt32>(_pktLen, pktLen);
    }

    void Header::setPktVersion(Poco::UInt16 pktVersion)
    {
        Utils::writeNumericField<Poco::UInt16>(_pktVersion, pktVersion);
    }

    void Header::setPktType(Poco::UInt16 pktType)
    {
        Utils::writeNumericField<Poco::UInt16>(_pktType, pktType);
    }

    void Header::setSessionId(Poco::UInt32 sessionId)
    {
        Utils::writeNumericField<Poco::UInt32>(_sessionId, sessionId);
    }

    // ===================================================================================
    #define CONSTRUCT_MESSAGE(name, t, v) \
    name##::##name() \
    { \
        _header.setPktVersion(v); \
        _header.setPktType(t); \
        _header.setSessionId(0); \
    }

    CONSTRUCT_MESSAGE(LoginRequest, PKT_TYPE_LOGIN, VERSION_NUM)
    CONSTRUCT_MESSAGE(LoginAccept, PKT_TYPE_LOGIN_ACCEPT, VERSION_NUM)
    CONSTRUCT_MESSAGE(LoginReject, PKT_TYPE_LOGIN_REJECT, VERSION_NUM)
	CONSTRUCT_MESSAGE(RefreshFingerprint, PKT_TYPE_REFRESH_FINGERPRINT, VERSION_NUM)

    CONSTRUCT_MESSAGE(AddPharmacistCertType, PKT_TYPE_ADD_PHARMACIST_CERT_TYPE, VERSION_NUM)
    CONSTRUCT_MESSAGE(AddPharmacistCertTypeSucc, PKT_TYPE_ADD_PHARMACIST_CERT_TYPE_SUCC, VERSION_NUM)
    CONSTRUCT_MESSAGE(AddPharmacistCertTypeFail, PKT_TYPE_ADD_PHARMACIST_CERT_TYPE_FAIL, VERSION_NUM)
    CONSTRUCT_MESSAGE(DeletePharmacists, PKT_TYPE_DELETE_PHARMACISTS, VERSION_NUM)
    CONSTRUCT_MESSAGE(DeletePharmacistsSucc, PKT_TYPE_DELETE_PHARMACISTS_SUCC, VERSION_NUM)
    CONSTRUCT_MESSAGE(DeletePharmacistsFail, PKT_TYPE_DELETE_PHARMACISTS_FAIL, VERSION_NUM)

    CONSTRUCT_MESSAGE(ListPharmacistCertTypes, PKT_TYPE_LIST_PHARMACIST_CERT_TYPES, VERSION_NUM)
    CONSTRUCT_MESSAGE(PharmacistCertTypes, PKT_TYPE_PHARMACIST_CERT_TYPES, VERSION_NUM)

	CONSTRUCT_MESSAGE(AddStore, PKT_TYPE_ADD_STORE, VERSION_NUM)
	CONSTRUCT_MESSAGE(AddStoreSucc, PKT_TYPE_ADD_STORE_SUCC, VERSION_NUM)
	CONSTRUCT_MESSAGE(AddStoreFail, PKT_TYPE_ADD_STORE_FAIL, VERSION_NUM)
	CONSTRUCT_MESSAGE(DeleteStores, PKT_TYPE_DELETE_STORES, VERSION_NUM)
	CONSTRUCT_MESSAGE(DeleteStoresSucc, PKT_TYPE_DELETE_STORES_SUCC, VERSION_NUM)
	CONSTRUCT_MESSAGE(DeleteStoresFail, PKT_TYPE_DELETE_STORES_FAIL, VERSION_NUM)

	CONSTRUCT_MESSAGE(ListStroes, PKT_TYPE_LIST_STORES, VERSION_NUM)
	CONSTRUCT_MESSAGE(Stores, PKT_TYPE_STORES, VERSION_NUM)

	CONSTRUCT_MESSAGE(PharmacistAdd, PKT_TYPE_PHARMACIST_ADD, VERSION_NUM)
	CONSTRUCT_MESSAGE(PharmacistAddSucc, PKT_TYPE_PHARMACIST_ADD_SUCC, VERSION_NUM)
	CONSTRUCT_MESSAGE(PharmacistAddFail, PKT_TYPE_PHARMACIST_ADD_FAIL, VERSION_NUM)

	CONSTRUCT_MESSAGE(PharmacistsBriefList, PKT_TYPE_PHARMACISTS_BRIEF_LIST, VERSION_NUM)
	CONSTRUCT_MESSAGE(PharmacistsBrief, PKT_TYPE_PHARMACISTS_BRIEF, VERSION_NUM)

    CONSTRUCT_MESSAGE(PharmacistsDetailList, PKT_TYPE_PHARMACISTS_DETAIL_LIST, VERSION_NUM)
    CONSTRUCT_MESSAGE(PharmacistsDetail, PKT_TYPE_PHARMACISTS_DETAIL, VERSION_NUM)

	CONSTRUCT_MESSAGE(PatientAdd, PKT_TYPE_PATIENT_ADD, VERSION_NUM)
	CONSTRUCT_MESSAGE(PatientAddSucc, PKT_TYPE_PATIENT_ADD_SUCC, VERSION_NUM)
	CONSTRUCT_MESSAGE(PatientAddFail, PKT_TYPE_PATIENT_ADD_FAIL, VERSION_NUM)

    CONSTRUCT_MESSAGE(GetPharmacistsDetailOfCurrentStore, PKT_TYPE_GET_PHARMACISTS_DETAIL_OF_CUR_STORE, VERSION_NUM)
    CONSTRUCT_MESSAGE(PharmacistsDetailOfCurrentStore, PKT_TYPE_PHARMACISTS_DETAIL_OF_CUR_STORE, VERSION_NUM)

	CONSTRUCT_MESSAGE(PharmacistRequest, PKT_TYPE_PHARMACIST_REQUEST, VERSION_NUM)
	CONSTRUCT_MESSAGE(PharmacistRequestSucc, PKT_TYPE_PHARMACIST_REQUEST_SUCC, VERSION_NUM)
    CONSTRUCT_MESSAGE(PharmacistRequestFail, PKT_TYPE_PHARMACIST_REQUEST_FAIL, VERSION_NUM)

	CONSTRUCT_MESSAGE(PatientConnected, PKT_TYPE_PATIENT_CONNECTED, VERSION_NUM)
    
    CONSTRUCT_MESSAGE(Prescription, PKT_TYPE_DATA_TRANSFER_PRESCRIPTION, VERSION_NUM)
    CONSTRUCT_MESSAGE(Video, PKT_TYPE_DATA_TRANSFER_VIDEO, VERSION_NUM)
    CONSTRUCT_MESSAGE(Audio, PKT_TYPE_DATA_TRANSFER_AUDIO, VERSION_NUM)

    CONSTRUCT_MESSAGE(StopConversation, PKT_TYPE_STOP_CONVERSATION, VERSION_NUM)
    CONSTRUCT_MESSAGE(PatientQuitService, PKT_TYPE_PATIENT_QUIT_CONVERSATION, VERSION_NUM)
    CONSTRUCT_MESSAGE(PharmacistQuitService, PKT_TYPE_PHARMACIST_QUIT_CONVERSATION, VERSION_NUM)

    CONSTRUCT_MESSAGE(AddDealOfPrescriptionDrug, PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG, VERSION_NUM)
    CONSTRUCT_MESSAGE(AddDealOfPrescriptionDrugSucc, PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG_SUCC, VERSION_NUM)
    CONSTRUCT_MESSAGE(AddDealOfPrescriptionDrugFail, PKT_TYPE_ADD_DEAL_OF_PRESCRIPTION_DRUG_FAIL, VERSION_NUM)
    
    CONSTRUCT_MESSAGE(ListDealsOfPrescriptionDrug, PKT_TYPE_LIST_DEALS_OF_PRESCRIPTION_DRUG, VERSION_NUM)
    CONSTRUCT_MESSAGE(DealsOfPrescriptionDrugDetail, PKT_TYPE_DEALS_OF_PRESCRIPTION_DRUG_DETAIL, VERSION_NUM)
    
    CONSTRUCT_MESSAGE(AddDealOfSpecialDrug, PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG, VERSION_NUM)
    CONSTRUCT_MESSAGE(AddDealOfSpecialDrugSucc, PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG_SUCC, VERSION_NUM)
    CONSTRUCT_MESSAGE(AddDealOfSpecialDrugFail, PKT_TYPE_ADD_DEAL_OF_SPECIAL_DRUG_FAIL, VERSION_NUM)

    CONSTRUCT_MESSAGE(ListDealsOfSpecialDrug, PKT_TYPE_LIST_DEALS_OF_SPECIAL_DRUG, VERSION_NUM)
    CONSTRUCT_MESSAGE(DealsOfSpecialDrugDetail, PKT_TYPE_DEALS_OF_SPECIAL_DRUG_DETAIL, VERSION_NUM)

    CONSTRUCT_MESSAGE(ListConsultingDetails, PKT_TYPE_LIST_CONSULTING_DETAILS, VERSION_NUM)
    CONSTRUCT_MESSAGE(ConsultingDetails, PKT_TYPE_CONSULTING_DETAILS, VERSION_NUM)

	CONSTRUCT_MESSAGE(ListPharmacistActivity, PKT_TYPE_LIST_PHARMACIST_ACTIVITY, VERSION_NUM)
	CONSTRUCT_MESSAGE(PharmacistActivity, PKT_TYPE_PHARMACIST_ACTIVITY, VERSION_NUM)
	
	CONSTRUCT_MESSAGE(ClientHeartbeat, PKT_TYPE_CLIENT_HEARTBEAT, VERSION_NUM)
    
    CONSTRUCT_MESSAGE(LogoutRequest, PKT_TYPE_LOGOUT, VERSION_NUM)

    CONSTRUCT_MESSAGE(ShutDownServer, PKT_TYPE_SHUTDOWN_SERVER, VERSION_NUM)
    
	// ===================================================================================
	Poco::UInt32 BinaryBlock::getContentLen() const
	{
		return Utils::readNumericField<Poco::UInt32>(_contentlen);
	}

	void BinaryBlock::setContentLen(Poco::UInt32 contentLen)
	{
		Utils::writeNumericField<Poco::UInt32>(_contentlen, contentLen);
	}

    // ===================================================================================
    void LoginRequest::setUsername(const std::string &username)
    {
        Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
    }

    void LoginRequest::setPassword(const std::string &password)
    {
        Utils::writeRightPaddedStr(_password, password.c_str(), sizeof(_password), password.size());
    }

    std::string LoginRequest::getUsername() const
    {
        return Utils::readRightPaddedStr(_username, sizeof(_username));
    }

    std::string LoginRequest::getPassword() const
    {
        return Utils::readRightPaddedStr(_password, sizeof(_password));
    }

    // ===================================================================================
    void LoginAccept::setUsername(const std::string &username)
    {
        Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
    }
    
    void LoginAccept::setUserId(Poco::UInt32 userId)
    {
        Utils::writeNumericField<Poco::UInt32>(_userId, userId);
    }

    void LoginAccept::setUserRole(Poco::UInt8 userRole)
    {
        _userRole = userRole;
    }

    void LoginAccept::setRegTime(const std::string &regTime)
	{
		Utils::writeRightPaddedStr(_regTime, regTime.c_str(), sizeof(_regTime), regTime.size());
	}

    std::string LoginAccept::getUsername() const
    {
        return Utils::readRightPaddedStr(_username, sizeof(_username));
    }

    Poco::UInt32 LoginAccept::getUserId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_userId);
    }

    Poco::UInt8 LoginAccept::getUserRole() const
    {
        return _userRole;
    }

    std::string LoginAccept::getRegTime() const
	{
		return Utils::readRightPaddedStr(_regTime, sizeof(_regTime));
	}

    // ===================================================================================
    void LoginReject::setUsername(const std::string &username)
    {
        Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
    }

    std::string LoginReject::getUsername() const
    {
        return Utils::readRightPaddedStr(_username, sizeof(_username));
    }

    // ===================================================================================
    void AddPharmacistCertType::setCertTypeName(const std::string &certTypeName)
    {
        Utils::writeRightPaddedStr(_certTypeName, certTypeName.c_str(), sizeof(_certTypeName), certTypeName.size());
    }

    std::string AddPharmacistCertType::getCertTypeName() const
    {
        return Utils::readRightPaddedStr(_certTypeName, sizeof(_certTypeName));
    }

    // ===================================================================================
    void AddPharmacistCertTypeSucc::setCertTypeName(const std::string &certTypeName)
    {
        Utils::writeRightPaddedStr(_certTypeName, certTypeName.c_str(), sizeof(_certTypeName), certTypeName.size());
    }

    std::string AddPharmacistCertTypeSucc::getCertTypeName() const
    {
        return Utils::readRightPaddedStr(_certTypeName, sizeof(_certTypeName));
    }

    // ===================================================================================
    void AddPharmacistCertTypeFail::setCertTypeName(const std::string &certTypeName)
    {
        Utils::writeRightPaddedStr(_certTypeName, certTypeName.c_str(), sizeof(_certTypeName), certTypeName.size());
    }

    std::string AddPharmacistCertTypeFail::getCertTypeName() const
    {
        return Utils::readRightPaddedStr(_certTypeName, sizeof(_certTypeName));
    }

    // ===================================================================================
    Poco::UInt32 PharmacistCertType_Item::getId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_id);
    }

    std::string PharmacistCertType_Item::getCertTypeName() const
    {
        return Utils::readRightPaddedStr(_certTypeName, sizeof(_certTypeName));
    }

    void PharmacistCertType_Item::setId(Poco::UInt32 id)
    {
        Utils::writeNumericField<Poco::UInt32>(_id, id);
    }

    void PharmacistCertType_Item::setCertTypeName(const std::string &certTypeName)
    {
        Utils::writeRightPaddedStr(_certTypeName, certTypeName.c_str(), sizeof(_certTypeName), certTypeName.size());
    }

    Poco::UInt32 PharmacistCertTypes::getNumOfCertTypes() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfCertTypes);
    }

    void PharmacistCertTypes::setNumOfCertTypes(Poco::UInt32 numOfCertTypes)
    {
        Utils::writeNumericField<Poco::UInt32>(_numOfCertTypes, numOfCertTypes);
    }

	// ===================================================================================
    Poco::UInt32 AddStore::getUpdateStoreId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_updateStoreId);
    }

    Poco::UInt8 AddStore::getNumOfPharmacists() const
	{
		return _numOfPharmacists;
	}
	
    void AddStore::setUpdateStoreId(Poco::UInt32 updateStoreId)
    {
        Utils::writeNumericField<Poco::UInt32>(_updateStoreId, updateStoreId);
    }

	void AddStore::setNumOfPharmacists(Poco::UInt8 numOfPharmacists)
	{
		_numOfPharmacists = numOfPharmacists;
	}

    // ===================================================================================
    Poco::UInt32 AddStoreSucc::getStoreId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_storeId);
    }

    void AddStoreSucc::setStoreId(Poco::UInt32 storeId)
    {
        Utils::writeNumericField<Poco::UInt32>(_storeId, storeId);
    }

	// ===================================================================================
	Poco::UInt32 DeleteStores::getNumOfStores() const
	{
		return Utils::readNumericField<Poco::UInt32>(_numOfStores);
	}

	void DeleteStores::setNumOfStores(Poco::UInt32 numOfStores)
	{
		Utils::writeNumericField<Poco::UInt32>(_numOfStores, numOfStores);
	}

    // ===================================================================================
    Poco::UInt32 Store_Item::getId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_id);
    }

    Poco::UInt32 Store_Item::getNumOfPatientAccts() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfPatientAccts);
    }

    void Store_Item::setId(Poco::UInt32 id)
    {
        Utils::writeNumericField<Poco::UInt32>(_id, id);
    }

    void Store_Item::setNumOfPatientAccts(Poco::UInt32 numOfPatientAccts)

    {
        Utils::writeNumericField<Poco::UInt32>(_numOfPatientAccts, numOfPatientAccts);
    }

    Poco::UInt32 Stores::getNumOfStores() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfStores);
    }

    void Stores::setNumOfStores(Poco::UInt32 numOfStores)
    {
        Utils::writeNumericField<Poco::UInt32>(_numOfStores, numOfStores);
    }

    // ===================================================================================
    void PharmacistAdd::setUsername(const std::string &username)
    {
        Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
    }

    void PharmacistAdd::setPassword(const std::string &password)
    {
        Utils::writeRightPaddedStr(_password, password.c_str(), sizeof(_password), password.size());
    }

    void PharmacistAdd::setRealName(const std::string &realName)
    {
        Utils::writeRightPaddedStr(_realName, realName.c_str(), sizeof(_realName), realName.size());
    }

    void PharmacistAdd::setCertTypeId(Poco::UInt32 certTypeId)
    {
        Utils::writeNumericField<Poco::UInt32>(_certTypeId, certTypeId);
    }

    void PharmacistAdd::setUpdatePharmacistId(Poco::UInt32 updatePharmacistId)
    {
        Utils::writeNumericField<Poco::UInt32>(_updatePharmacistId, updatePharmacistId);
    }

    std::string PharmacistAdd::getUsername() const
    {
        return Utils::readRightPaddedStr(_username, sizeof(_username));
    }

    std::string PharmacistAdd::getPassword() const
    {
        return Utils::readRightPaddedStr(_password, sizeof(_password));
    }

    std::string PharmacistAdd::getRealName() const
    {
        return Utils::readRightPaddedStr(_realName, sizeof(_realName));
    }

    Poco::UInt32  PharmacistAdd::getCertTypeId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_certTypeId);
    }

    Poco::UInt32 PharmacistAdd::getUpdatePharmacistId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_updatePharmacistId);
    }

	// ===================================================================================
	void PharmacistAddSucc::setUsername(const std::string &username)
	{
		Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
	}

	std::string PharmacistAddSucc::getUsername() const
	{
		return Utils::readRightPaddedStr(_username, sizeof(_username));
	}

	// ===================================================================================
	void PharmacistAddFail::setUsername(const std::string &username)
	{
		Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
	}

	std::string PharmacistAddFail::getUsername() const
	{
		return Utils::readRightPaddedStr(_username, sizeof(_username));
	}

    // ===================================================================================
	Poco::UInt32 DeletePharmacists::getNumOfPharmacists() const
	{
		return Utils::readNumericField<Poco::UInt32>(_numOfPharmacists);
	}

    void DeletePharmacists::setNumOfPharmacists(Poco::UInt32 numOfPharmacists)
	{
		Utils::writeNumericField<Poco::UInt32>(_numOfPharmacists, numOfPharmacists);
	}

	// ===================================================================================
	Poco::UInt32 PharmacistsBrief::getNumOfPharmacists() const
	{
		return Utils::readNumericField<Poco::UInt32>(_numOfPharmacists);
	}

	void PharmacistsBrief::setNumOfPharmacists(Poco::UInt32 numOfPharmacists)
	{
		Utils::writeNumericField<Poco::UInt32>(_numOfPharmacists, numOfPharmacists);
	}

	// ===================================================================================
	bool PharmacistsBriefList::getIncludePhoto() const
	{
		return (_includePhoto == 1);
	}

	void PharmacistsBriefList::setIncludePhoto(bool includePhoto)
	{
		_includePhoto = (includePhoto ? 1 : 0);
	}

	// ===================================================================================
	Poco::UInt32 PharmacistsBrief_Item::getUserId() const
	{
		return Utils::readNumericField<Poco::UInt32>(_userId);
	}

	Poco::UInt32 PharmacistsBrief_Item::getStoreId() const
	{
		return Utils::readNumericField<Poco::UInt32>(_storeId);
	}

	std::string PharmacistsBrief_Item::getRealName() const
	{
		return Utils::readRightPaddedStr(_realName, sizeof(_realName));
	}

    Poco::UInt32 PharmacistsBrief_Item::getCertTypeId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_certTypeId);
    }

	Poco::UInt8 PharmacistsBrief_Item::getStatus() const
	{
		return _status;
	}

	bool PharmacistsBrief_Item::getHasPhoto() const
	{
		return (_hasPhoto == 1);
	}

	void PharmacistsBrief_Item::setUserId(Poco::UInt32 userId)
	{
		Utils::writeNumericField<Poco::UInt32>(_userId, userId);
	}

	void PharmacistsBrief_Item::setStoreId(Poco::UInt32 storeId)
	{
		Utils::writeNumericField<Poco::UInt32>(_storeId, storeId);
	}

	void PharmacistsBrief_Item::setRealName(const std::string &realName)
	{
		Utils::writeRightPaddedStr(_realName, realName.c_str(), sizeof(_realName), realName.size());
	}

    void PharmacistsBrief_Item::setCertTypeId(Poco::UInt32 certTypeId)
    {
        Utils::writeNumericField<Poco::UInt32>(_certTypeId, certTypeId);
    }

	void PharmacistsBrief_Item::setStatus(Poco::UInt8 status)
	{
		_status = status;
	}

	void PharmacistsBrief_Item::setHasPhoto(bool hasPhoto)
	{
		_hasPhoto = (hasPhoto ? 1 : 0);
	}

	// ===================================================================================
	Poco::UInt32 PharmacistsDetailList::getUserId() const
	{
		return Utils::readNumericField<Poco::UInt32>(_userId);
	}

	void PharmacistsDetailList::setUserId(Poco::UInt32 userId)
	{
		Utils::writeNumericField<Poco::UInt32>(_userId, userId);
	}

    // ===================================================================================
    Poco::UInt32 PharmacistsDetail::getNumOfPharmacists() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfPharmacists);
    }

    void PharmacistsDetail::setNumOfPharmacists(Poco::UInt32 numOfPharmacists)
    {
        Utils::writeNumericField<Poco::UInt32>(_numOfPharmacists, numOfPharmacists);
    }

    // ===================================================================================
    Poco::UInt32 PharmacistsDetail_Item::getUserId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_userId);
    }

    std::string PharmacistsDetail_Item::getUsername() const
    {
        return Utils::readRightPaddedStr(_username, sizeof(_username));
    }

	Poco::UInt8 PharmacistsDetail_Item::getNumOfCoveredStores() const
    {
        return _numOfCoveredStores;
    }

    std::string PharmacistsDetail_Item::getRealName() const
    {
        return Utils::readRightPaddedStr(_realName, sizeof(_realName));
    }

    Poco::UInt32 PharmacistsDetail_Item::getCertTypeId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_certTypeId);
    }

    Poco::UInt8 PharmacistsDetail_Item::getStatus() const
    {
        return _status;
    }

    void PharmacistsDetail_Item::setUserId(Poco::UInt32 userId)
    {
        Utils::writeNumericField<Poco::UInt32>(_userId, userId);
    }

    void PharmacistsDetail_Item::setUsername(const std::string &username)
    {
        Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
    }

    void PharmacistsDetail_Item::setNumOfCoveredStores(Poco::UInt8 numOfCoveredStores)
    {
        _numOfCoveredStores = numOfCoveredStores;
    }

    void PharmacistsDetail_Item::setRealName(const std::string &realName)
    {
        Utils::writeRightPaddedStr(_realName, realName.c_str(), sizeof(_realName), realName.size());
    }

    void PharmacistsDetail_Item::setCertTypeId(Poco::UInt32 certTypeId)
    {
        Utils::writeNumericField<Poco::UInt32>(_certTypeId, certTypeId);
    }

    void PharmacistsDetail_Item::setStatus(Poco::UInt8 status)
    {
        _status = status;
    }

	// ===================================================================================
    void PatientAdd::setIsUpdate(bool isUpdate)
    {
        _isUpdate = isUpdate ? 1 : 0;
    }
    
    void PatientAdd::setUsername(const std::string &username)
	{
		Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
	}

	void PatientAdd::setPassword(const std::string &password)
	{
		Utils::writeRightPaddedStr(_password, password.c_str(), sizeof(_password), password.size());
	}

	void PatientAdd::setStoreId(Poco::UInt32 storeId)
	{
		Utils::writeNumericField<Poco::UInt32>(_storeId, storeId);
	}

    bool PatientAdd::getIsUpdate() const
    {
        return (_isUpdate == 1);
    }
    
    std::string PatientAdd::getUsername() const
	{
		return Utils::readRightPaddedStr(_username, sizeof(_username));
	}

	std::string PatientAdd::getPassword() const
	{
		return Utils::readRightPaddedStr(_password, sizeof(_password));
	}

	Poco::UInt32 PatientAdd::getStoreId() const
	{
		return Utils::readNumericField<Poco::UInt32>(_storeId);
	}

	// ===================================================================================
	void PatientAddSucc::setUsername(const std::string &username)
	{
		Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
	}

	std::string PatientAddSucc::getUsername() const
	{
		return Utils::readRightPaddedStr(_username, sizeof(_username));
	}

	// ===================================================================================
	void PatientAddFail::setUsername(const std::string &username)
	{
		Utils::writeRightPaddedStr(_username, username.c_str(), sizeof(_username), username.size());
	}

	std::string PatientAddFail::getUsername() const
	{
		return Utils::readRightPaddedStr(_username, sizeof(_username));
	}

    // ===================================================================================
    Poco::UInt32 PharmacistsDetailOfCurrentStore::getNumOfPharmacists() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfPharmacists);
    }

    void PharmacistsDetailOfCurrentStore::setNumOfPharmacists(Poco::UInt32 numOfPharmacists)
    {
        Utils::writeNumericField<Poco::UInt32>(_numOfPharmacists, numOfPharmacists);
    }
    
    // ===================================================================================
    void PharmacistsDetailOfCurrentStore_Item::setRealName(const std::string &realName)
    {
        Utils::writeRightPaddedStr(_realName, realName.c_str(), sizeof(_realName), realName.size());
    }

    void PharmacistsDetailOfCurrentStore_Item::setCertTypeId(Poco::UInt32 certTypeId)
	{
		Utils::writeNumericField<Poco::UInt32>(_certTypeId, certTypeId);
	}

    std::string PharmacistsDetailOfCurrentStore_Item::getRealName() const
    {
        return Utils::readRightPaddedStr(_realName, sizeof(_realName));
    }

    Poco::UInt32 PharmacistsDetailOfCurrentStore_Item::getCertTypeId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_certTypeId);
    }

    // ===================================================================================
	void PharmacistRequest::setCertTypeId(Poco::UInt32 certTypeId)
	{
		Utils::writeNumericField<Poco::UInt32>(_certTypeId, certTypeId);
	}
	
	void PharmacistRequest::setUsePool(bool usePool)
    {
        _usePool = (usePool ? 1 : 0);
    }

	Poco::UInt32  PharmacistRequest::getCertTypeId() const
	{
		return Utils::readNumericField<Poco::UInt32>(_certTypeId);
	}

    bool PharmacistRequest::getUsePool() const
    {
        return (_usePool == 1);
    }

    // ===================================================================================
    Poco::UInt32 PharmacistRequestSucc::getPharmacistSessionId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_pharmacistSessionId);
    }

    void PharmacistRequestSucc::setPharmacistSessionId(Poco::UInt32 pharmacistSessionId)
    {
        Utils::writeNumericField<Poco::UInt32>(_pharmacistSessionId, pharmacistSessionId);
    }

    // ===================================================================================
    Poco::UInt32 PatientConnected::getPatientSessionId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_patientSessionId);
    }

    void PatientConnected::setPatientSessionId(Poco::UInt32 patientSessionId)
    {
        Utils::writeNumericField<Poco::UInt32>(_patientSessionId, patientSessionId);
    }

    // ===================================================================================
    Poco::UInt32 PatientQuitService::getPatientSessionId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_patientSessionId);
    }

    void PatientQuitService::setPatientSessionId(Poco::UInt32 patientSessionId)
    {
        Utils::writeNumericField<Poco::UInt32>(_patientSessionId, patientSessionId);
    }

    // ===================================================================================
    Poco::UInt32 PharmacistQuitService::getPharmacistSessionId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_pharmacistSessionId);
    }

    void PharmacistQuitService::setPharmacistSessionId(Poco::UInt32 pharmacistSessionId)
    {
        Utils::writeNumericField<Poco::UInt32>(_pharmacistSessionId, pharmacistSessionId);
    }
    
    // ===================================================================================
	Poco::UInt32 AddDealOfPrescriptionDrug::getUpdateDealId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_updateDealId);
    }
    
    Poco::UInt8 AddDealOfPrescriptionDrug::getBuyerAge() const
    {
        return _buyerAge;
    }

    bool AddDealOfPrescriptionDrug::getBuyerIsMale() const
    {
        return (_buyerIsMale == 1);
    }

	void AddDealOfPrescriptionDrug::setUpdateDealId(Poco::UInt32 updateDealId)
    {
        Utils::writeNumericField<Poco::UInt32>(_updateDealId, updateDealId);
    }

    void AddDealOfPrescriptionDrug::setBuyerAge(Poco::UInt8 buyerAge)
    {
        _buyerAge = buyerAge;
    }

    void AddDealOfPrescriptionDrug::setBuyerIsMale(bool buyerIsMale)
    {
        _buyerIsMale = (buyerIsMale ? 1 : 0);
    }

    // ===================================================================================
    void ListDealsOfPrescriptionDrug::setStoreId(Poco::UInt32 storeId)
    {
        Utils::writeNumericField<Poco::UInt32>(_storeId, storeId);
    }

    void ListDealsOfPrescriptionDrug::setStartTime(const std::string &startTime)
    {
        Utils::writeRightPaddedStr(_startTime, startTime.c_str(), sizeof(_startTime), startTime.size());
    }

    void ListDealsOfPrescriptionDrug::setEndTime(const std::string &endTime)
    {
        Utils::writeRightPaddedStr(_endTime, endTime.c_str(), sizeof(_endTime), endTime.size());
    }

    Poco::UInt32 ListDealsOfPrescriptionDrug::getStoreId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_storeId);
    }

    std::string ListDealsOfPrescriptionDrug::getStartTime() const
    {
        return Utils::readRightPaddedStr(_startTime, sizeof(_startTime));
    }

    std::string ListDealsOfPrescriptionDrug::getEndTime() const
    {
        return Utils::readRightPaddedStr(_endTime, sizeof(_endTime));
    }

    // ===================================================================================
    Poco::UInt32 DealsOfPrescriptionDrugDetail_Item::getDealId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_dealId);
    }

    Poco::UInt8 DealsOfPrescriptionDrugDetail_Item::getBuyerAge() const
    {
        return _buyerAge;
    }

    bool DealsOfPrescriptionDrugDetail_Item::getBuyerIsMale() const
    {
        return (_buyerIsMale == 1);
    }

    std::string DealsOfPrescriptionDrugDetail_Item::getTime() const
    {
        return Utils::readRightPaddedStr(_time, sizeof(_time));
    }

    void DealsOfPrescriptionDrugDetail_Item::setDealId(Poco::UInt32 dealId)
    {
        Utils::writeNumericField<Poco::UInt32>(_dealId, dealId);
    }

    void DealsOfPrescriptionDrugDetail_Item::setBuyerAge(Poco::UInt8 buyerAge)
    {
        _buyerAge = buyerAge;
    }

    void DealsOfPrescriptionDrugDetail_Item::setBuyerIsMale(bool buyerIsMale)
    {
        _buyerIsMale = (buyerIsMale ? 1 : 0);
    }

    void DealsOfPrescriptionDrugDetail_Item::setTime(const std::string &time)
    {
        Utils::writeRightPaddedStr(_time, time.c_str(), sizeof(_time), time.size());
    }

    Poco::UInt32 DealsOfPrescriptionDrugDetail::getNumOfDeals() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfDeals);
    }

    void DealsOfPrescriptionDrugDetail::setNumOfDeals(Poco::UInt32 numOfDeals)
    {
        Utils::writeNumericField<Poco::UInt32>(_numOfDeals, numOfDeals);
    }

    // ===================================================================================
    Poco::UInt32 AddDealOfSpecialDrug::getUpdateDealId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_updateDealId);
    }

    Poco::UInt8 AddDealOfSpecialDrug::getBuyerAge() const
    {
        return _buyerAge;
    }

    bool AddDealOfSpecialDrug::getBuyerIsMale() const
    {
        return (_buyerIsMale == 1);
    }

    void AddDealOfSpecialDrug::setUpdateDealId(Poco::UInt32 updateDealId)
    {
        Utils::writeNumericField<Poco::UInt32>(_updateDealId, updateDealId);
    }

    void AddDealOfSpecialDrug::setBuyerAge(Poco::UInt8 buyerAge)
    {
        _buyerAge = buyerAge;
    }

    void AddDealOfSpecialDrug::setBuyerIsMale(bool buyerIsMale)
    {
        _buyerIsMale = (buyerIsMale ? 1 : 0);
    }

    // ===================================================================================
    void ListDealsOfSpecialDrug::setStoreId(Poco::UInt32 storeId)
    {
        Utils::writeNumericField<Poco::UInt32>(_storeId, storeId);
    }

    void ListDealsOfSpecialDrug::setStartTime(const std::string &startTime)
    {
        Utils::writeRightPaddedStr(_startTime, startTime.c_str(), sizeof(_startTime), startTime.size());
    }

    void ListDealsOfSpecialDrug::setEndTime(const std::string &endTime)
    {
        Utils::writeRightPaddedStr(_endTime, endTime.c_str(), sizeof(_endTime), endTime.size());
    }

    Poco::UInt32 ListDealsOfSpecialDrug::getStoreId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_storeId);
    }

    std::string ListDealsOfSpecialDrug::getStartTime() const
    {
        return Utils::readRightPaddedStr(_startTime, sizeof(_startTime));
    }

    std::string ListDealsOfSpecialDrug::getEndTime() const
    {
        return Utils::readRightPaddedStr(_endTime, sizeof(_endTime));
    }

    // ===================================================================================
    Poco::UInt32 DealsOfSpecialDrugDetail_Item::getDealId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_dealId);
    }

    Poco::UInt8 DealsOfSpecialDrugDetail_Item::getBuyerAge() const
    {
        return _buyerAge;
    }

    bool DealsOfSpecialDrugDetail_Item::getBuyerIsMale() const
    {
        return (_buyerIsMale == 1);
    }

    std::string DealsOfSpecialDrugDetail_Item::getTime() const
    {
        return Utils::readRightPaddedStr(_time, sizeof(_time));
    }

    void DealsOfSpecialDrugDetail_Item::setDealId(Poco::UInt32 dealId)
    {
        Utils::writeNumericField<Poco::UInt32>(_dealId, dealId);
    }

    void DealsOfSpecialDrugDetail_Item::setBuyerAge(Poco::UInt8 buyerAge)
    {
        _buyerAge = buyerAge;
    }

    void DealsOfSpecialDrugDetail_Item::setBuyerIsMale(bool buyerIsMale)
    {
        _buyerIsMale = (buyerIsMale ? 1 : 0);
    }

    void DealsOfSpecialDrugDetail_Item::setTime(const std::string &time)
    {
        Utils::writeRightPaddedStr(_time, time.c_str(), sizeof(_time), time.size());
    }

    Poco::UInt32 DealsOfSpecialDrugDetail::getNumOfDeals() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfDeals);
    }

    void DealsOfSpecialDrugDetail::setNumOfDeals(Poco::UInt32 numOfDeals)
    {
        Utils::writeNumericField<Poco::UInt32>(_numOfDeals, numOfDeals);
    }

    // ===================================================================================
    void ListConsultingDetails::setStoreId(Poco::UInt32 storeId)
    {
        Utils::writeNumericField<Poco::UInt32>(_storeId, storeId);
    }

    void ListConsultingDetails::setStartTime(const std::string &startTime)
    {
        Utils::writeRightPaddedStr(_startTime, startTime.c_str(), sizeof(_startTime), startTime.size());
    }

    void ListConsultingDetails::setEndTime(const std::string &endTime)
    {
        Utils::writeRightPaddedStr(_endTime, endTime.c_str(), sizeof(_endTime), endTime.size());
    }

    Poco::UInt32 ListConsultingDetails::getStoreId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_storeId);
    }

    std::string ListConsultingDetails::getStartTime() const
    {
        return Utils::readRightPaddedStr(_startTime, sizeof(_startTime));
    }

    std::string ListConsultingDetails::getEndTime() const
    {
        return Utils::readRightPaddedStr(_endTime, sizeof(_endTime));
    }
	
    // ===================================================================================
    Poco::UInt32 ConsultingDetails_Item::getPharmacistId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_pharmacistId);
    }

    Poco::UInt32 ConsultingDetails_Item::getPatientId() const
    {
        return Utils::readNumericField<Poco::UInt32>(_patientId);
    }

    std::string ConsultingDetails_Item::getStartTime() const
    {
        return Utils::readRightPaddedStr(_startTime, sizeof(_startTime));
    }

    std::string ConsultingDetails_Item::getEndTime() const
    {
        return Utils::readRightPaddedStr(_endTime, sizeof(_endTime));
    }

    Poco::UInt32 ConsultingDetails_Item::getNumOfPrescriptions() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfPrescriptions);
    }
    
    void ConsultingDetails_Item::setPharmacistId(Poco::UInt32 pharmacistId)
    {
        Utils::writeNumericField<Poco::UInt32>(_pharmacistId, pharmacistId);
    }

    void ConsultingDetails_Item::setPatientId(Poco::UInt32 patientId)
    {
        Utils::writeNumericField<Poco::UInt32>(_patientId, patientId);
    }

    void ConsultingDetails_Item::setStartTime(const std::string &startTime)
    {
        Utils::writeRightPaddedStr(_startTime, startTime.c_str(), sizeof(_startTime), startTime.size());
    }

    void ConsultingDetails_Item::setEndTime(const std::string &endTime)
    {
        Utils::writeRightPaddedStr(_endTime, endTime.c_str(), sizeof(_endTime), endTime.size());
    }

    void ConsultingDetails_Item::setNumOfPrescriptions(Poco::UInt32 numOfPrescriptions)
    {
        Utils::writeNumericField<Poco::UInt32>(_numOfPrescriptions, numOfPrescriptions);
    }

    Poco::UInt32 ConsultingDetails::getNumOfItems() const
    {
        return Utils::readNumericField<Poco::UInt32>(_numOfItems);
    }

    void ConsultingDetails::setNumOfItems(Poco::UInt32 numOfItems)
    {
        Utils::writeNumericField<Poco::UInt32>(_numOfItems, numOfItems);
    }

	// ===================================================================================
	void ListPharmacistActivity::setPharmacistId(Poco::UInt32 pharmacistId)
	{
		Utils::writeNumericField<Poco::UInt32>(_pharmacistId, pharmacistId);
	}

	void ListPharmacistActivity::setStartTime(const std::string &startTime)
	{
		Utils::writeRightPaddedStr(_startTime, startTime.c_str(), sizeof(_startTime), startTime.size());
	}

	void ListPharmacistActivity::setEndTime(const std::string &endTime)
	{
		Utils::writeRightPaddedStr(_endTime, endTime.c_str(), sizeof(_endTime), endTime.size());
	}

	Poco::UInt32 ListPharmacistActivity::getPharmacistId() const
	{
		return Utils::readNumericField<Poco::UInt32>(_pharmacistId);
	}

	std::string ListPharmacistActivity::getStartTime() const
	{
		return Utils::readRightPaddedStr(_startTime, sizeof(_startTime));
	}

	std::string ListPharmacistActivity::getEndTime() const
	{
		return Utils::readRightPaddedStr(_endTime, sizeof(_endTime));
	}

	// ===================================================================================
	Poco::UInt32 PharmacistActivity::getPharmacistId() const
	{
		return Utils::readNumericField<Poco::UInt32>(_pharmacistId);
	}

	Poco::UInt32 PharmacistActivity::getNumOfItems() const
	{
		return Utils::readNumericField<Poco::UInt32>(_numOfItems);
	}

	void PharmacistActivity::setPharmacistId(Poco::UInt32 pharmacistId)
	{
		Utils::writeNumericField<Poco::UInt32>(_pharmacistId, pharmacistId);
	}

	void PharmacistActivity::setNumOfItems(Poco::UInt32 numOfItems)
	{
		Utils::writeNumericField<Poco::UInt32>(_numOfItems, numOfItems);
	}
} }
