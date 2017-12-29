#pragma once

#include <string>
#include <vector>
#include "User.h"

namespace client {

    class UserCallback
    {
    public:     
        //============================================================================
        // 所有回调传入的指针都是不安全的，可能在函数返回后被后台释放
        //============================================================================

        // 登录成功
        struct UserInfo
        {
            std::string                  _username;
            unsigned int                 _userId;
            sserver::server::USER_ROLE   _userRole;
            std::string                  _regTime;
            unsigned int                 _fpLen;
            const char                   *_pFingerprint;
            unsigned int                 _signatureLen;
            const char                   *_pSignature;
        };
        virtual void onLoginAccpted(const UserInfo &info) {}

        // 登录失败
        virtual void onLoginRejected(const std::string &username) {}

        // 添加药剂师证书类型成功
        virtual void onPharmacistCertTypeAdded(const std::string &certTypeName) {}

        // 添加药剂师证书类型失败
        virtual void onPharmacistCertTypeAddFail(const std::string &certTypeName) {}

        // 所有药剂师证书类型列表
        struct PharmacistCertTypeItem
        {
            unsigned int _id;
            std::string  _certTypeName;
        };
        virtual void onPharmacistCertTypes(const std::vector<PharmacistCertTypeItem> &types) {}

        // 添加或修改药剂师成功
        virtual void onPharmacistAdded(const std::string &username) {}

        // 添加或修改药剂师失败
        virtual void onPharmacistAddFail(const std::string &username) {}

        // 删除一个或多个药剂师成功
        virtual void onPharmacistsDeleted() {}

        // 删除一个或多个药剂师失败
        virtual void onPharmacistsDeleteFail() {}

        // 所有药剂师的基本信息
        struct PharmacistBriefItem
        {
            unsigned int                 _userId;
            std::string                  _realName;
            unsigned int                 _certTypeId;
            sserver::server::USER_STATUS _status;
            unsigned int                 _photoLen;
            const char                   *_pPhoto;
        };
        virtual void onPharmacistsBrief(const std::vector<PharmacistBriefItem> &info) {}

        // 所有药剂师的详细信息
        struct PharmacistDetailItem
        {
            unsigned int                 _userId;
            std::string                  _username;
            std::string                  _shenfenzheng;
            std::string                  _realName;
            unsigned int                 _certTypeId;
            sserver::server::USER_STATUS _status;
			std::set<unsigned int>       _coveredStoreIds;
            unsigned int                 _certLen;
            const char                   *_pCert;
            unsigned int                 _fpLen;
            const char                   *_pFingerprint;
            unsigned int                 _signatureLen;
            const char                   *_pSignature;
            unsigned int                 _introductionLen;
            const char                   *_pIntroduction;
            unsigned int                 _photoLen;
            const char                   *_pPhoto;
        };
        virtual void onPharmacistsDetail(const std::vector<PharmacistDetailItem> &info) {}

        // 添加或修改药店成功
        virtual void onStoreAdded(const std::string &name, unsigned int storeId) {}

        // 添加或修改药店失败
        virtual void onStoreAddFail(const std::string &name) {}

		// 删除一个或多个药店成功
		virtual void onStoresDeleted() {}

		// 删除一个或多个药店失败
		virtual void onStoresDeleteFail() {}

        // 所有药店列表
        struct StoreItem
        {
            unsigned int          _id;
            std::string           _name;
            std::string           _addr;
            std::set<std::string> _patientAccts;
        };
        virtual void onStores(const std::vector<StoreItem> &stores) {}

        // 药店账号添加成功
        virtual void onPatientAdded(const std::string &username) {}

        // 药店账号添加失败
        virtual void onPatientAddFail(const std::string &username) {}

        // 当前药店的药剂师信息
        struct PharmacistOfCurrentStore
        {
            std::string                  _realName;
            unsigned int                 _certTypeId;
            unsigned int                 _introductionLen;
            const char                   *_pIntroduction;
            unsigned int                 _photoLen;
            const char                   *_pPhoto;
        };
        virtual void onPharmacistsDetailOfCurStore(const std::vector<PharmacistOfCurrentStore> &info) {}

        // 找到一个药剂师进行服务
        virtual void onPharmacistRequestSucc(unsigned int pharmacistSessionId) {}

        // 未找到药剂师进行服务
        virtual void onPharmacistRequestFail() {}

        // 通知药剂师，已经与某个病人搭上线
        virtual void onPatientConnected(unsigned int patientSessionId) {}

        // 处方单数据
        virtual void onPrescription(const User::Prescription &prescription) {}

        // 视频数据
        virtual void onVideo(const User::Video &video) {}

        // 音频数据
        virtual void onAudio(const User::Audio &audio) {}

        // 通知药剂师，正在对话的病人退出对话
        virtual void onPatientQuitService(unsigned int patientSessionId) {}

        // 通知病人，正在对话的药剂师退出对话
        virtual void onPharmacistQuitService(unsigned int pharmacistSessionId) {}

		// 记录或更新处方药销售记录成功
		virtual void onDealOfPrescriptionDrugAdded() {}

		// 记录或更新处方药销售记录失败
		virtual void onDealOfPrescriptionDrugAddFail() {}

        // 处方药销售记录查询结果
        struct DealsOfPrescriptionDrugDetailItem
        {
            unsigned int  _dealId;
            std::string   _buyerName;
            unsigned int  _buyerAge;
            bool          _buyerIsMale;
            std::string   _time;
            std::string   _drugHuoHao;
            std::string   _drugMingCheng;
            std::string   _drugPiHao;
            std::string   _drugGuiGe;
            std::string   _drugJiLiang;
            std::string   _drugShengChanChangJia;
            std::string   _drugChuFangLaiYuan;
        };
        virtual void onDealsOfPrescriptionDrugDetail(const std::vector<DealsOfPrescriptionDrugDetailItem> &info) {}

        // 记录或更新特殊药销售记录成功
        virtual void onDealOfSpecialDrugAdded() {}

        // 记录或更新特殊药销售记录失败
        virtual void onDealOfSpecialDrugAddFail() {}

        // 特殊药销售记录查询结果
        struct DealsOfSpecialDrugDetailItem
        {
            unsigned int  _dealId;
            std::string   _buyerName;
            std::string   _buyerShenFenZheng;
            unsigned int  _buyerAge;
            bool          _buyerIsMale;
            std::string   _time;
            std::string   _drugHuoHao;
            std::string   _drugMingCheng;
            std::string   _drugPiHao;
            std::string   _drugGuiGe;
            std::string   _drugJiLiang;
            std::string   _drugGouMaiShuLiang;
            std::string   _drugShengChanChangJia;
            std::string   _drugChuFangLaiYuan;
        };
        virtual void onDealsOfSpecialDrugDetail(const std::vector<DealsOfSpecialDrugDetailItem> &info) {}

		// 远程咨询/审方记录
		struct ConsultingDetailItem
		{
			unsigned int               _pharmacistId;
			unsigned int               _patientId;
			std::string                _startTime;
			std::string                _endTime;
			std::string                _pharmacistVideoLoc;
            std::string                _patientVideoLoc;
            std::string                _pharmacistAudioLoc;
            std::string                _patientAudioLoc;
			std::vector<std::string>   _prescriptionLocs;
		};
		virtual void onConsultingDetails(const std::vector<ConsultingDetailItem> &info) {}

		// 药师活动记录
		virtual void onPharmacistActivityList(const std::vector<std::string> &info) {}
    };

} // namespace client