#pragma once

#include <string>
#include <vector>
#include "User.h"

namespace client {

    class UserCallback
    {
    public:     
        //============================================================================
        // ���лص������ָ�붼�ǲ���ȫ�ģ������ں������غ󱻺�̨�ͷ�
        //============================================================================

        // ��¼�ɹ�
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

        // ��¼ʧ��
        virtual void onLoginRejected(const std::string &username) {}

        // ���ҩ��ʦ֤�����ͳɹ�
        virtual void onPharmacistCertTypeAdded(const std::string &certTypeName) {}

        // ���ҩ��ʦ֤������ʧ��
        virtual void onPharmacistCertTypeAddFail(const std::string &certTypeName) {}

        // ����ҩ��ʦ֤�������б�
        struct PharmacistCertTypeItem
        {
            unsigned int _id;
            std::string  _certTypeName;
        };
        virtual void onPharmacistCertTypes(const std::vector<PharmacistCertTypeItem> &types) {}

        // ��ӻ��޸�ҩ��ʦ�ɹ�
        virtual void onPharmacistAdded(const std::string &username) {}

        // ��ӻ��޸�ҩ��ʦʧ��
        virtual void onPharmacistAddFail(const std::string &username) {}

        // ɾ��һ������ҩ��ʦ�ɹ�
        virtual void onPharmacistsDeleted() {}

        // ɾ��һ������ҩ��ʦʧ��
        virtual void onPharmacistsDeleteFail() {}

        // ����ҩ��ʦ�Ļ�����Ϣ
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

        // ����ҩ��ʦ����ϸ��Ϣ
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

        // ��ӻ��޸�ҩ��ɹ�
        virtual void onStoreAdded(const std::string &name, unsigned int storeId) {}

        // ��ӻ��޸�ҩ��ʧ��
        virtual void onStoreAddFail(const std::string &name) {}

		// ɾ��һ������ҩ��ɹ�
		virtual void onStoresDeleted() {}

		// ɾ��һ������ҩ��ʧ��
		virtual void onStoresDeleteFail() {}

        // ����ҩ���б�
        struct StoreItem
        {
            unsigned int          _id;
            std::string           _name;
            std::string           _addr;
            std::set<std::string> _patientAccts;
        };
        virtual void onStores(const std::vector<StoreItem> &stores) {}

        // ҩ���˺���ӳɹ�
        virtual void onPatientAdded(const std::string &username) {}

        // ҩ���˺����ʧ��
        virtual void onPatientAddFail(const std::string &username) {}

        // ��ǰҩ���ҩ��ʦ��Ϣ
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

        // �ҵ�һ��ҩ��ʦ���з���
        virtual void onPharmacistRequestSucc(unsigned int pharmacistSessionId) {}

        // δ�ҵ�ҩ��ʦ���з���
        virtual void onPharmacistRequestFail() {}

        // ֪ͨҩ��ʦ���Ѿ���ĳ�����˴�����
        virtual void onPatientConnected(unsigned int patientSessionId) {}

        // ����������
        virtual void onPrescription(const User::Prescription &prescription) {}

        // ��Ƶ����
        virtual void onVideo(const User::Video &video) {}

        // ��Ƶ����
        virtual void onAudio(const User::Audio &audio) {}

        // ֪ͨҩ��ʦ�����ڶԻ��Ĳ����˳��Ի�
        virtual void onPatientQuitService(unsigned int patientSessionId) {}

        // ֪ͨ���ˣ����ڶԻ���ҩ��ʦ�˳��Ի�
        virtual void onPharmacistQuitService(unsigned int pharmacistSessionId) {}

		// ��¼����´���ҩ���ۼ�¼�ɹ�
		virtual void onDealOfPrescriptionDrugAdded() {}

		// ��¼����´���ҩ���ۼ�¼ʧ��
		virtual void onDealOfPrescriptionDrugAddFail() {}

        // ����ҩ���ۼ�¼��ѯ���
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

        // ��¼���������ҩ���ۼ�¼�ɹ�
        virtual void onDealOfSpecialDrugAdded() {}

        // ��¼���������ҩ���ۼ�¼ʧ��
        virtual void onDealOfSpecialDrugAddFail() {}

        // ����ҩ���ۼ�¼��ѯ���
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

		// Զ����ѯ/�󷽼�¼
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

		// ҩʦ���¼
		virtual void onPharmacistActivityList(const std::vector<std::string> &info) {}
    };

} // namespace client