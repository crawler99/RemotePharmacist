#pragma once

#include "CommonMessages.h"
#include <string>

namespace sserver { namespace message {

#pragma pack(push, 1)

    // =======================================================================================
    struct LoginRequest
    {
        LoginRequest();
        // --------------------------------------------------------
        std::string   getUsername() const;
        std::string   getPassword() const;
        // --------------------------------------------------------
        void          setUsername(const std::string &username);
        void          setPassword(const std::string &password);
        // --------------------------------------------------------
        Header        _header;

    private:
        char          _username[64];
        char          _password[64];
		// => binary block: _fingerprint
    };

	// =======================================================================================
	struct RefreshFingerprint
	{
		RefreshFingerprint();
		// --------------------------------------------------------
		Header        _header;
	};

    // =======================================================================================
    struct AddPharmacistCertType
    {
        AddPharmacistCertType();
        // --------------------------------------------------------
        std::string   getCertTypeName() const;	
        // --------------------------------------------------------
        void          setCertTypeName(const std::string &certTypeName);
		// --------------------------------------------------------
		Header        _header;

    private:
        char          _certTypeName[128];
    };

    // =======================================================================================
    struct ListPharmacistCertTypes
    {
        ListPharmacistCertTypes();
        // --------------------------------------------------------
        Header        _header;
    };

	// =======================================================================================
	struct AddStore
	{
		AddStore();
		// --------------------------------------------------------
		Poco::UInt32  getUpdateStoreId() const;
        Poco::UInt8   getNumOfPharmacists() const;
		// --------------------------------------------------------
        void          setUpdateStoreId(Poco::UInt32 updateStoreId);
        void          setNumOfPharmacists(Poco::UInt8 numOfPharmacists);
		// --------------------------------------------------------
		Header        _header;
        
	private:
        Poco::UInt32  _updateStoreId;
		Poco::UInt8   _numOfPharmacists;
		// => pharmacist Ids
		// => binary block: store_name
		// => binary block: store_address
	};

	// =======================================================================================
	struct DeleteStores
	{
		DeleteStores();
		// --------------------------------------------------------
		Poco::UInt32  getNumOfStores() const;
		// --------------------------------------------------------
		void          setNumOfStores(Poco::UInt32 numOfStores);
		// --------------------------------------------------------
		Header        _header;

	private:
		Poco::UInt32  _numOfStores;
		// => store Ids
	};

	// =======================================================================================
	struct ListStroes
	{
		ListStroes();
		// --------------------------------------------------------
		Header        _header;
	};

	// =======================================================================================
	struct PharmacistAdd
	{
		PharmacistAdd();
		// --------------------------------------------------------
		std::string   getUsername() const;	
		std::string   getPassword() const;
		std::string   getRealName() const;
        Poco::UInt32  getCertTypeId() const;
        Poco::UInt32  getUpdatePharmacistId() const;
		// --------------------------------------------------------
		void          setUsername(const std::string &username);
		void          setPassword(const std::string &password);
		void          setRealName(const std::string &realName);
        void          setCertTypeId(Poco::UInt32 certTypeId);
        void          setUpdatePharmacistId(Poco::UInt32 updatePharmacistId);
		// --------------------------------------------------------
		Header        _header;

	private:
		Poco::UInt32  _updatePharmacistId;
		char          _username[64];
		char          _password[64];
		char          _realName[64];
        Poco::UInt32  _certTypeId;
		// => binary block: _shenfenzheng
		// => binary block: _certificate
		// => binary block: _fingerprint
        // => binary block: _signature
		// => binary block: _introduction
		// => binary block: _photo
	};

    // =======================================================================================
    struct DeletePharmacists
    {
        DeletePharmacists();
        // --------------------------------------------------------
        Poco::UInt32  getNumOfPharmacists() const;
        // --------------------------------------------------------
        void          setNumOfPharmacists(Poco::UInt32 numOfPharmacists);
		// --------------------------------------------------------
		Header        _header;

    private:
        Poco::UInt32  _numOfPharmacists;
        // => pharmacist Ids
    };

	// =======================================================================================
	struct PharmacistsBriefList
	{
		PharmacistsBriefList();
		// --------------------------------------------------------
		bool          getIncludePhoto() const;
		// --------------------------------------------------------
		void          setIncludePhoto(bool includePhoto);
		// --------------------------------------------------------
		Header        _header;

	private:
		Poco::UInt8   _includePhoto;
	};

    // =======================================================================================
    struct PharmacistsDetailList
    {
        PharmacistsDetailList();
        // --------------------------------------------------------
		Poco::UInt32  getUserId() const;
		// --------------------------------------------------------
		void          setUserId(Poco::UInt32 userId);
        // --------------------------------------------------------
		Header        _header;

	private:
		Poco::UInt32  _userId;  // if set to 0, then means list all
    };

	// =======================================================================================
	struct PatientAdd
	{
		PatientAdd();
		// --------------------------------------------------------
		bool          getIsUpdate() const;
        std::string   getUsername() const;	
		std::string   getPassword() const;
		Poco::UInt32  getStoreId() const;
        // --------------------------------------------------------
        void          setIsUpdate(bool isUpdate);
		void          setUsername(const std::string &username);
		void          setPassword(const std::string &password);
		void          setStoreId(Poco::UInt32 storeId);
		// --------------------------------------------------------
		Header        _header;

	private:
        Poco::UInt8   _isUpdate;
		char          _username[64];
		char          _password[64];
		Poco::UInt32  _storeId;
	};

    // =======================================================================================
    struct GetPharmacistsDetailOfCurrentStore
    {
        GetPharmacistsDetailOfCurrentStore();
        // --------------------------------------------------------
        Header        _header;   
    };

	// =======================================================================================
	struct PharmacistRequest
	{
		PharmacistRequest();
        // --------------------------------------------------------
		Poco::UInt32  getCertTypeId() const;
		bool          getUsePool() const;
		// --------------------------------------------------------
		void          setCertTypeId(Poco::UInt32 certTypeId);
        void          setUsePool(bool usePool);
        // --------------------------------------------------------
		Header        _header;

    private:
		Poco::UInt32  _certTypeId;
        Poco::UInt8   _usePool;
	};

    // =======================================================================================
    struct AddDealOfPrescriptionDrug
    {
        AddDealOfPrescriptionDrug();
        // --------------------------------------------------------
        Poco::UInt32  getUpdateDealId() const;
        Poco::UInt8   getBuyerAge() const;
        bool          getBuyerIsMale() const;
        // --------------------------------------------------------
        void          setUpdateDealId(Poco::UInt32 updateDealId);
        void          setBuyerAge(Poco::UInt8 buyerAge);
        void          setBuyerIsMale(bool buyerIsMale);
        // --------------------------------------------------------
        Header        _header;

    private:
        Poco::UInt32  _updateDealId;
        Poco::UInt8   _buyerAge;
        Poco::UInt8   _buyerIsMale;
        // => binary block: buyer_name 
        // => binary block: drug_huohao
        // => binary block: drug_mingcheng
        // => binary block: drug_pihao
        // => binary block: drug_guige
        // => binary block: drug_jiliang
        // => binary block: drug_shengchanchangjia
        // => binary block: drug_chufanglaiyuan
    };

    // =======================================================================================
    struct ListDealsOfPrescriptionDrug
    {
        ListDealsOfPrescriptionDrug();
        // --------------------------------------------------------
        Poco::UInt32  getStoreId() const;
        std::string   getStartTime() const;
        std::string   getEndTime() const;
        // --------------------------------------------------------
        void          setStoreId(Poco::UInt32 storeId);
        void          setStartTime(const std::string &startTime);
        void          setEndTime(const std::string &endTime);
        // --------------------------------------------------------
        Header        _header;

    private:
        Poco::UInt32  _storeId;
        char          _startTime[32];    // "YYYY-MM-DD hh:mm:ss"
        char          _endTime[32];      // "YYYY-MM-DD hh:mm:ss"
    };

    // =======================================================================================
    struct AddDealOfSpecialDrug
    {
        AddDealOfSpecialDrug();
        // --------------------------------------------------------
        Poco::UInt32  getUpdateDealId() const;
        Poco::UInt8   getBuyerAge() const;
        bool          getBuyerIsMale() const;
        // --------------------------------------------------------
        void          setUpdateDealId(Poco::UInt32 updateDealId);
        void          setBuyerAge(Poco::UInt8 buyerAge);
        void          setBuyerIsMale(bool buyerIsMale);
        // --------------------------------------------------------
        Header        _header;

    private:
        Poco::UInt32  _updateDealId;
        Poco::UInt8   _buyerAge;
        Poco::UInt8   _buyerIsMale;
        // => binary block: buyer_name
        // => binary block: buyer_shenfenzheng
        // => binary block: drug_huohao
        // => binary block: drug_mingcheng
        // => binary block: drug_pihao
        // => binary block: drug_guige
        // => binary block: drug_jiliang
        // => binary block: drug_goumaishuliang
        // => binary block: drug_shengchanchangjia
        // => binary block: drug_chufanglaiyuan
    };

    // =======================================================================================
    struct ListDealsOfSpecialDrug
    {
        ListDealsOfSpecialDrug();
        // --------------------------------------------------------
        Poco::UInt32  getStoreId() const;
        std::string   getStartTime() const;
        std::string   getEndTime() const;
        // --------------------------------------------------------
        void          setStoreId(Poco::UInt32 storeId);
        void          setStartTime(const std::string &startTime);
        void          setEndTime(const std::string &endTime);
        // --------------------------------------------------------
        Header        _header;

    private:
        Poco::UInt32  _storeId;
        char          _startTime[32];    // "YYYY-MM-DD hh:mm:ss"
        char          _endTime[32];      // "YYYY-MM-DD hh:mm:ss"
    };

    // =======================================================================================
    struct ListConsultingDetails
    {
        ListConsultingDetails();
        // --------------------------------------------------------
        Poco::UInt32  getStoreId() const;
        std::string   getStartTime() const;
        std::string   getEndTime() const;
        // --------------------------------------------------------
        void          setStoreId(Poco::UInt32 storeId);
        void          setStartTime(const std::string &startTime);
        void          setEndTime(const std::string &endTime);
        // --------------------------------------------------------
        Header        _header;

    private:
        Poco::UInt32  _storeId;
        char          _startTime[32];    // "YYYY-MM-DD hh:mm:ss"
        char          _endTime[32];      // "YYYY-MM-DD hh:mm:ss"
    };

	// =======================================================================================
	struct ListPharmacistActivity
	{
		ListPharmacistActivity();
		// --------------------------------------------------------
		Poco::UInt32  getPharmacistId() const;
		std::string   getStartTime() const;
		std::string   getEndTime() const;
		// --------------------------------------------------------
		void          setPharmacistId(Poco::UInt32 pharmacistId);
		void          setStartTime(const std::string &startTime);
		void          setEndTime(const std::string &endTime);
		// --------------------------------------------------------
		Header        _header;

	private:
		Poco::UInt32  _pharmacistId;
		char          _startTime[32];    // "YYYY-MM-DD hh:mm:ss"
		char          _endTime[32];      // "YYYY-MM-DD hh:mm:ss"
	};

	// =======================================================================================
	struct ClientHeartbeat
	{
		ClientHeartbeat();
		// --------------------------------------------------------
		Header        _header;
	};

	// =======================================================================================
	struct StopConversation
	{
		StopConversation();
		// --------------------------------------------------------
		Header        _header;
	};

	// =======================================================================================
    struct LogoutRequest
    {
        LogoutRequest();
        // --------------------------------------------------------
        Header        _header;
    };

    // =======================================================================================
    struct ShutDownServer
    {
        ShutDownServer();
        // --------------------------------------------------------
        Header        _header;
    };

#pragma pack(pop)

} }
