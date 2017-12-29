#pragma once

#include "CommonMessages.h"
#include <string>

namespace sserver { namespace message { 

#pragma pack(push, 1)

    // =======================================================================================
	// response to LoginRequest
	// =======================================================================================
    struct LoginAccept
    {
        LoginAccept();
        // --------------------------------------------------------
		std::string     getUsername() const;
        Poco::UInt32    getUserId() const;
		Poco::UInt8     getUserRole() const;
		std::string     getRegTime() const;
        // --------------------------------------------------------
		void            setUsername(const std::string &username);
        void            setUserId(Poco::UInt32 userId);
		void            setUserRole(Poco::UInt8 userRole);
		void            setRegTime(const std::string &regTime);
        // --------------------------------------------------------
        Header          _header;

    private:
        char            _username[64];
        Poco::UInt32    _userId;
        Poco::UInt8     _userRole;
		char            _regTime[32];
        // => binary block: _fingerprint
        // => binary block: _signature
    };

    struct LoginReject
    {
        LoginReject();
        // --------------------------------------------------------
        std::string     getUsername() const;
        // --------------------------------------------------------
        void            setUsername(const std::string &username);
        // --------------------------------------------------------
        Header          _header;

    private:
        char            _username[64];
    };

    // =======================================================================================
    // response to AddPharmacistCertType
    // =======================================================================================
    struct AddPharmacistCertTypeSucc
    {
        AddPharmacistCertTypeSucc();
        // --------------------------------------------------------
        std::string     getCertTypeName() const;	
        // --------------------------------------------------------
        void            setCertTypeName(const std::string &certTypeName);
        // --------------------------------------------------------
        Header          _header;

    private:
        char            _certTypeName[128];
    };

    struct AddPharmacistCertTypeFail
    {
        AddPharmacistCertTypeFail();
        // --------------------------------------------------------
        std::string     getCertTypeName() const;
        // --------------------------------------------------------
        void            setCertTypeName(const std::string &certTypeName);
        // --------------------------------------------------------
        Header          _header;

    private:
        char            _certTypeName[128];
    };

    // =======================================================================================
    // response to DeletePharmacists
    // =======================================================================================
    struct DeletePharmacistsSucc
    {
        DeletePharmacistsSucc();
        // --------------------------------------------------------
        Header          _header;
        BinaryBlock     _name;
    };

    struct DeletePharmacistsFail
    {
        DeletePharmacistsFail();
        // --------------------------------------------------------
        Header          _header;
        BinaryBlock     _name;
    };

    // =======================================================================================
    // response to ListPharmacistCertTypes
    // =======================================================================================
    struct PharmacistCertType_Item
    {
        // ----------------------------------------------------------------------	
        Poco::UInt32    getId() const;
        std::string     getCertTypeName() const;
        // ----------------------------------------------------------------------
        void            setId(Poco::UInt32 id);
        void            setCertTypeName(const std::string &certTypeName);

    private:
        Poco::UInt32    _id;
        char            _certTypeName[128];
    };

    struct PharmacistCertTypes
    {
        PharmacistCertTypes();
        // ----------------------------------------------------------------------
        Poco::UInt32    getNumOfCertTypes() const;
        // ----------------------------------------------------------------------
        void            setNumOfCertTypes(Poco::UInt32 numOfCertTypes);
        // ----------------------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _numOfCertTypes;
    };

	// =======================================================================================
	// response to AddStore
	// =======================================================================================
	struct AddStoreSucc
	{
		AddStoreSucc();
		// --------------------------------------------------------
        Poco::UInt32    getStoreId() const;
        // --------------------------------------------------------
        void            setStoreId(Poco::UInt32 storeId);
        // --------------------------------------------------------
		Header          _header;

    private:
        Poco::UInt32    _storeId;
        // => binary block: store_name
	};

	struct AddStoreFail
	{
		AddStoreFail();
		// --------------------------------------------------------
		Header          _header;

    private:
		// => binary block: store_name
	};

	// =======================================================================================
	// response to DeleteStores
	// =======================================================================================
	struct DeleteStoresSucc
	{
		DeleteStoresSucc();
		// --------------------------------------------------------
		Header          _header;
		BinaryBlock     _name;
	};

	struct DeleteStoresFail
	{
		DeleteStoresFail();
		// --------------------------------------------------------
		Header          _header;
		BinaryBlock     _name;
	};

	// =======================================================================================
    // response to ListStroes
	// =======================================================================================
    struct Store_Item
    {
        // ----------------------------------------------------------------------	
        Poco::UInt32    getId() const;
        Poco::UInt32    getNumOfPatientAccts() const;
        // ----------------------------------------------------------------------
        void            setId(Poco::UInt32 id);
        void            setNumOfPatientAccts(Poco::UInt32 numOfPatientAccts);

    private:
        Poco::UInt32    _id;
        Poco::UInt32    _numOfPatientAccts;
        // => binary block:        store_name 
        // => binary block:        store_address
        // => binary blocks:       patient_account
    };

    struct Stores
    {
        Stores();
        // ----------------------------------------------------------------------
        Poco::UInt32    getNumOfStores() const;
        // ----------------------------------------------------------------------
        void            setNumOfStores(Poco::UInt32 numOfStores);
        // ----------------------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _numOfStores;
    };

	// =======================================================================================
	// response to PharmacistAdd
	// =======================================================================================
	struct PharmacistAddSucc
	{
		PharmacistAddSucc();
		// --------------------------------------------------------
		std::string     getUsername() const;
		// --------------------------------------------------------
		void            setUsername(const std::string &username);
		// --------------------------------------------------------
		Header          _header;

	private:
		char            _username[64];
	};

	struct PharmacistAddFail
	{
		PharmacistAddFail();
		// --------------------------------------------------------
		std::string     getUsername() const;
		// --------------------------------------------------------
		void            setUsername(const std::string &username);
		// --------------------------------------------------------
		Header          _header;

	private:
		char            _username[64];
	};

	// =======================================================================================
	// response to PharmacistsBriefList
	// =======================================================================================
	struct PharmacistsBrief_Item
	{
		// ----------------------------------------------------------------------	
		Poco::UInt32    getUserId() const;
		Poco::UInt32    getStoreId() const;
		std::string     getRealName() const;
        Poco::UInt32    getCertTypeId() const;
		Poco::UInt8     getStatus() const;
		bool            getHasPhoto() const;
		// ----------------------------------------------------------------------
		void            setUserId(Poco::UInt32 userId);
		void            setStoreId(Poco::UInt32 storeId);
		void            setRealName(const std::string &realName);
        void            setCertTypeId(Poco::UInt32 certTypeId);
		void            setStatus(Poco::UInt8 status);
		void            setHasPhoto(bool hasPhoto);

	private:
		Poco::UInt32    _userId;
		Poco::UInt32    _storeId;
		char            _realName[64];
        Poco::UInt32    _certTypeId;
		Poco::UInt8     _status;
		Poco::UInt8     _hasPhoto;
	};

	struct PharmacistsBrief
	{
		PharmacistsBrief();
		// ----------------------------------------------------------------------
		Poco::UInt32    getNumOfPharmacists() const;
		// ----------------------------------------------------------------------
		void            setNumOfPharmacists(Poco::UInt32 numOfPharmacists);
		// --------------------------------------------------------
		Header          _header;

	private:
		Poco::UInt32    _numOfPharmacists;
	};

    // =======================================================================================
    // response to PharmacistsDetailList
    // =======================================================================================
    struct PharmacistsDetail_Item
    {
        // ----------------------------------------------------------------------	
        Poco::UInt32    getUserId() const;
        std::string     getUsername() const;
        std::string     getRealName() const;
        Poco::UInt32    getCertTypeId() const;
        Poco::UInt8     getStatus() const;
        Poco::UInt8     getNumOfCoveredStores() const;
        // ----------------------------------------------------------------------
        void            setUserId(Poco::UInt32 userId);
        void            setUsername(const std::string &username);
        void            setRealName(const std::string &realName);
        void            setCertTypeId(Poco::UInt32 certTypeId);
        void            setStatus(Poco::UInt8 status);
		void            setNumOfCoveredStores(Poco::UInt8 numOfCoveredStores);

    private:
        Poco::UInt32    _userId;
        char            _username[64];
        char            _realName[64];
        Poco::UInt32    _certTypeId;
        Poco::UInt8     _status;
		Poco::UInt8     _numOfCoveredStores;
        // => binary block: _shenfenzheng
        // => binary block: _certificate
        // => binary block: _fingerprint
        // => binary block: _signature
        // => covered store Ids
        // => binary block: _introduction
        // => binary block: _photo
    };

    struct PharmacistsDetail
    {
        PharmacistsDetail();
        // ----------------------------------------------------------------------
        Poco::UInt32    getNumOfPharmacists() const;
        // ----------------------------------------------------------------------
        void            setNumOfPharmacists(Poco::UInt32 numOfPharmacists);
        // --------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _numOfPharmacists;
    };

	// =======================================================================================
	// response to PatientAdd
	// =======================================================================================
	struct PatientAddSucc
	{
		PatientAddSucc();
		// --------------------------------------------------------
		std::string     getUsername() const;
		// --------------------------------------------------------
		void            setUsername(const std::string &username);
		// --------------------------------------------------------
		Header          _header;

	private:
		char            _username[64];
	};

	struct PatientAddFail
	{
		PatientAddFail();
		// --------------------------------------------------------
		std::string     getUsername() const;
		// --------------------------------------------------------
		void            setUsername(const std::string &username);
		// --------------------------------------------------------
		Header          _header;

	private:
		char            _username[64];
	};

    // =======================================================================================
    // response to GetPharmacistsDetailOfCurrentStore
    // =======================================================================================
    struct PharmacistsDetailOfCurrentStore_Item
    {
        // ----------------------------------------------------------------------	
        std::string     getRealName() const;
        Poco::UInt32    getCertTypeId() const;
        // ----------------------------------------------------------------------
        void            setRealName(const std::string &realName);
        void            setCertTypeId(Poco::UInt32 certTypeId);

    private:
        char            _realName[64];
        Poco::UInt32    _certTypeId;
        // => binary block: _introduction
        // => binary block: _photo
    };
    
    struct PharmacistsDetailOfCurrentStore
    {
        PharmacistsDetailOfCurrentStore();
        // ----------------------------------------------------------------------
        Poco::UInt32    getNumOfPharmacists() const;
        // ----------------------------------------------------------------------
        void            setNumOfPharmacists(Poco::UInt32 numOfPharmacists);
        // --------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _numOfPharmacists;
    };

	// =======================================================================================
	// response to PharmacistRequest
	// =======================================================================================
    struct PharmacistRequestFail
    {
        PharmacistRequestFail();
        // --------------------------------------------------------
        Header          _header;
    };
    
    struct PharmacistRequestSucc
    {
        PharmacistRequestSucc();
        // ----------------------------------------------------------------------
        Poco::UInt32    getPharmacistSessionId() const;
        // ----------------------------------------------------------------------
        void            setPharmacistSessionId(Poco::UInt32 pharmacistSessionId);
        // ----------------------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _pharmacistSessionId;
    };

	// =======================================================================================
	// notification to the pharmacist that a patient is waiting for service
	// =======================================================================================
    struct PatientConnected
    {
        PatientConnected();
        // ----------------------------------------------------------------------
        Poco::UInt32    getPatientSessionId() const;
        // ----------------------------------------------------------------------
        void            setPatientSessionId(Poco::UInt32 patientSessionId);
        // ----------------------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _patientSessionId;
    };

    // =======================================================================================
    // notification to the pharmacist that a patient has stopped the conversation
    // =======================================================================================
    struct PatientQuitService
    {
        PatientQuitService();
        // ----------------------------------------------------------------------
        Poco::UInt32    getPatientSessionId() const;
        // ----------------------------------------------------------------------
        void            setPatientSessionId(Poco::UInt32 patientSessionId);
        // ----------------------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _patientSessionId;
    };

    // =======================================================================================
    // notification to the patient that a pharmacist has stopped the conversation
    // =======================================================================================
    struct PharmacistQuitService
    {
        PharmacistQuitService();
        // ----------------------------------------------------------------------
        Poco::UInt32    getPharmacistSessionId() const;
        // ----------------------------------------------------------------------
        void            setPharmacistSessionId(Poco::UInt32 pharmacistSessionId);
        // ----------------------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _pharmacistSessionId;
    };

    // =======================================================================================
    // response to AddDealOfPrescriptionDrug
    // =======================================================================================
    struct AddDealOfPrescriptionDrugSucc
    {
        AddDealOfPrescriptionDrugSucc();
        // --------------------------------------------------------
        Header          _header;
        BinaryBlock     _name;
    };

    struct AddDealOfPrescriptionDrugFail
    {
        AddDealOfPrescriptionDrugFail();
        // --------------------------------------------------------
        Header          _header;
        BinaryBlock     _name;
    };

    // =======================================================================================
    // response to ListDealsOfPrescriptionDrug
    // =======================================================================================
    struct DealsOfPrescriptionDrugDetail_Item
    {
        // --------------------------------------------------------
        Poco::UInt32  getDealId() const;
        Poco::UInt8   getBuyerAge() const;
        bool          getBuyerIsMale() const;
        std::string   getTime() const;
        // --------------------------------------------------------
        void          setDealId(Poco::UInt32 dealId);
        void          setBuyerAge(Poco::UInt8 buyerAge);
        void          setBuyerIsMale(bool buyerIsMale);
        void          setTime(const std::string &time);

    private:
        Poco::UInt32  _dealId;
        Poco::UInt8   _buyerAge;
        Poco::UInt8   _buyerIsMale;
        char          _time[32];
        // => binary block: buyer_name 
        // => binary block: drug_huohao
        // => binary block: drug_mingcheng
        // => binary block: drug_pihao
        // => binary block: drug_guige
        // => binary block: drug_jiliang
        // => binary block: drug_shengchanchangjia
        // => binary block: drug_chufanglaiyuan
    };

    struct DealsOfPrescriptionDrugDetail
    {
        DealsOfPrescriptionDrugDetail();
        // ----------------------------------------------------------------------
        Poco::UInt32    getNumOfDeals() const;
        // ----------------------------------------------------------------------
        void            setNumOfDeals(Poco::UInt32 numOfDeals);
        // --------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _numOfDeals;
    };

    // =======================================================================================
    // response to AddDealOfSpecialDrug
    // =======================================================================================
    struct AddDealOfSpecialDrugSucc
    {
        AddDealOfSpecialDrugSucc();
        // --------------------------------------------------------
        Header          _header;
        BinaryBlock     _name;
    };

    struct AddDealOfSpecialDrugFail
    {
        AddDealOfSpecialDrugFail();
        // --------------------------------------------------------
        Header          _header;
        BinaryBlock     _name;
    };

    // =======================================================================================
    // response to ListDealsOfSpecialDrug
    // =======================================================================================
    struct DealsOfSpecialDrugDetail_Item
    {
        // --------------------------------------------------------
        Poco::UInt32  getDealId() const;
        Poco::UInt8   getBuyerAge() const;
        bool          getBuyerIsMale() const;
        std::string   getTime() const;
        // --------------------------------------------------------
        void          setDealId(Poco::UInt32 dealId);
        void          setBuyerAge(Poco::UInt8 buyerAge);
        void          setBuyerIsMale(bool buyerIsMale);
        void          setTime(const std::string &time);

    private:
        Poco::UInt32  _dealId;
        Poco::UInt8   _buyerAge;
        Poco::UInt8   _buyerIsMale;
        char          _time[32];
        // => binary block: buyer_name 
        // => binary block: buyer_shenfengzheng 
        // => binary block: drug_huohao
        // => binary block: drug_mingcheng
        // => binary block: drug_pihao
        // => binary block: drug_guige
        // => binary block: drug_jiliang
        // => binary block: drug_goumaishuliang
        // => binary block: drug_shengchanchangjia
        // => binary block: drug_chufanglaiyuan
    };

    struct DealsOfSpecialDrugDetail
    {
        DealsOfSpecialDrugDetail();
        // ----------------------------------------------------------------------
        Poco::UInt32    getNumOfDeals() const;
        // ----------------------------------------------------------------------
        void            setNumOfDeals(Poco::UInt32 numOfDeals);
        // --------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _numOfDeals;
    };

    // =======================================================================================
    // response to ListConsultingDetails
    // =======================================================================================
    struct ConsultingDetails_Item
    {
        // --------------------------------------------------------
        Poco::UInt32  getPharmacistId() const;
        Poco::UInt32  getPatientId() const;
        std::string   getStartTime() const;
        std::string   getEndTime() const;
        Poco::UInt32  getNumOfPrescriptions() const;
        // --------------------------------------------------------
        void          setPharmacistId(Poco::UInt32 pharmacistId);
        void          setPatientId(Poco::UInt32 patientId);
        void          setStartTime(const std::string &startTime);
        void          setEndTime(const std::string &endTime);
        void          setNumOfPrescriptions(Poco::UInt32 numOfPrescriptions);

    private:
        Poco::UInt32  _pharmacistId;
        Poco::UInt32  _patientId;
        char          _startTime[32];
        char          _endTime[32];
        Poco::UInt32  _numOfPrescriptions;
        // => binary block: pharmacist_video_loc 
        // => binary block: patient_video_loc 
        // => binary block: pharmacist_audio_loc
        // => binary block: patient_audio_loc
        // => prescription locs
    };

    struct ConsultingDetails
    {
        ConsultingDetails();
        // ----------------------------------------------------------------------
        Poco::UInt32    getNumOfItems() const;
        // ----------------------------------------------------------------------
        void            setNumOfItems(Poco::UInt32 numOfItems);
        // --------------------------------------------------------
        Header          _header;

    private:
        Poco::UInt32    _numOfItems;
    };

	// =======================================================================================
	// response to ListPharmacistActivity
	// =======================================================================================
	struct PharmacistActivity
	{
		PharmacistActivity();
		// --------------------------------------------------------
		Poco::UInt32    getPharmacistId() const;
		Poco::UInt32    getNumOfItems() const;
		// ----------------------------------------------------------------------
		void            setPharmacistId(Poco::UInt32 pharmacistId);
		void            setNumOfItems(Poco::UInt32 numOfItems);
		// --------------------------------------------------------
		Header          _header;

	private:
		Poco::UInt32    _pharmacistId;
		Poco::UInt32    _numOfItems;
		// => activity timestamps
	};

#pragma pack(pop)

} }
