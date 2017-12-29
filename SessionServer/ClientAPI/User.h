#pragma once

#include <string>
#include <vector>
#include <set>
#include "../SessionServer/server/Enums.h"

namespace client {

    class UserImpl;
    class UserCallback;

    class User
    {
    public:
        User(const std::string &username, 
             const std::string &password,
             UserCallback *userCallback);
        
		// ���ӷ�����
        bool connect(const std::string &serverIP, unsigned int serverPort);
		
		// ===============================================================================
		// �����ķ���
		// ===============================================================================
		
		// ���������Ϣѭ��
        void startReceiving();

		// ���������Ƶ��ѭ��
		void startReceivingVideo();

		// ���������Ƶ��ѭ��
		void startReceivingAudio();

		// ===============================================================================
		// �������ķ���
		// ===============================================================================

        // ֹͣ������Ϣ
        void stopReceiving();

		// ֹͣ������Ƶ��
		void stopReceivingVideo();

		// ֹͣ������Ƶ��
		void stopReceivingAudio();
        
        // �����¼
		bool req_Login(const char *pFingerprint, unsigned int fpLen);
        bool req_Login_Sync(const char *pFingerprint, unsigned int fpLen);

		// ��ˢָ��
		bool send_RefreshFingerprint();

        // �������һ��ҩ��ʦ֤�����ͣ���ҩ/��ҩ/...��
        // certTypeName:       ҩ��ʦ֤����������
        bool req_AddPharmacistCertType(const std::string &certTypeName);
        bool req_AddPharmacistCertType_Sync(const std::string &certTypeName);

        // ��������ҩ��ʦ֤�������б�
        bool req_ListPharmacistCertTypes();
        bool req_ListPharmacistCertTypes_Sync();

		// �������һ��ҩ��ʦ
		// username:           ҩ��ʦ�û���
		// password:           ҩ��ʦ����
		// shenfenzheng:       ҩʦ���֤
		// realName:           ҩ��ʦ����
        // certTypeId:         ҩ��ʦ֤������Id
        // certLen             ҩ��ʦ֤���С �����벻Ϊ0��
        // pCert               ҩ��ʦ֤������ָ��
        // fpLen               ҩ��ʦָ�ƴ�С �����벻Ϊ0��
        // pFingerprint        ҩ��ʦָ������ָ��
        // signatureLen        ҩ��ʦǩ����С �����벻Ϊ0��
        // pSignature          ҩ��ʦǩ������ָ��
		// introductionLen:    ҩ��ʦ�������ֳ��� (�������Ϊ0�����ʾû�н���)
		// pIntroduction:      ҩ��ʦ��������ָ��
		// photoLen:           ҩ��ʦ��Ƭ��С ���������Ϊ0�����ʾû����Ƭ��
		// pPhoto:             ҩ��ʦ��Ƭ����ָ��
        struct PharmacistRegInfo
        {
            unsigned int      _updatePharmacistId;   // ���Ϊ0���ʾ��ӣ������ʾҪ�޸ĵ�Id
            std::string       _username;
            std::string       _password;
			std::string       _shenfenzheng;
            std::string       _realName;
            unsigned int      _certTypeId;
            unsigned int      _certLen;
            const char        *_pCert;
            unsigned int      _fpLen;
            const char        *_pFingerprint;
            unsigned int      _signatureLen;
            const char        *_pSignature;
            unsigned int      _introductionLen;
            const char        *_pIntroduction;
            unsigned int      _photoLen;
            const char        *_pPhoto;
        };
		bool req_AddPharmacist(const PharmacistRegInfo &info);
        bool req_AddPharmacist_Sync(const PharmacistRegInfo &info);

        // ����ɾ��һ������ҩʦ
        // pharmacistIds:      Ҫɾ����ҩʦId����
        bool req_DeletePharmacists(const std::set<unsigned int>  &pharmacistIds);
        bool req_DeletePharmacists_Sync(const std::set<unsigned int>  &pharmacistIds);

		// ��������ҩ��ʦ�Ļ�����Ϣ
        bool req_ListPharmacists(bool includePhoto = false);
		bool req_ListPharmacists_Sync(bool includePhoto = false);

        // ��������ҩ��ʦ����ϸ��Ϣ
		// userId:             Ŀ��ҩ��ʦ����Id���������Ϊ0�����ʾ����ȫ����ҩ��ʦ
        bool req_ListPharmacistsDetail(unsigned int userId = 0);
        bool req_ListPharmacistsDetail_Sync(unsigned int userId = 0);

		// �������һ��ҩ��
		// name:               ҩ������
		// addr:               ҩ���ַ
		// pharmacistIds:      ��������ҩ��ʦ��Id
        struct StoreRegInfo
        {
            unsigned int            _updateStoreId;    // ���Ϊ0���ʾ��ӣ������ʾҪ�޸ĵ�Id
            std::string             _name;
            std::string             _addr;
			std::set<unsigned int>  _pharmacistIds;
        };
		bool req_AddStore(const StoreRegInfo &info);
        bool req_AddStore_Sync(const StoreRegInfo &info);

		// ����ɾ��һ������ҩ��
		// storeIds:           Ҫɾ����ҩ��Id����
		bool req_DeleteStores(const std::set<unsigned int>  &storeIds);
		bool req_DeleteStores_Sync(const std::set<unsigned int>  &storeIds);
		
		// ��������ҩ���б�
		bool req_ListStores();
        bool req_ListStores_Sync();
        
		// �������һ��ҩ���˺�
        // isUpdate:  �Ƿ�Ϊ�޸�����ֻ���޸����룩
		// username:  �˺���
		// password:  �˺�����
		// storeId:   ҩ��ID����ҩ���б���ѡȡ
        struct StoreAccountRegInfo
        {
            bool         _isUpdate;
            std::string  _username;
            std::string  _password;
            unsigned int _storeId;
        };
		bool req_AddStoreAccount(const StoreAccountRegInfo &info);
        bool req_AddStoreAccount_Sync(const StoreAccountRegInfo &info);

        // ����ǰҩ���ҩ��ʦ��Ϣ
        bool req_PharmacistsDetailOfCurStore();
        bool req_PharmacistsDetailOfCurStore_Sync();

		// �ն�����ҩ��ʦ����
		// certTypeId: ҩ��ʦ֤������Id
        // usePool:    �Ƿ񲻹��ĵ�ǰҩ�������ҩʦ����ֱ�Ӵ�ȫ�ֿ���ҩʦ����Ѱ��
        bool req_Pharmacist(unsigned int certTypeId, bool usePool);
        bool req_Pharmacist_Sync(unsigned int certTypeId, bool usePool);

        // ���䴦��������
        // dataLen:    ���������ݳ���
        // pData:      ����������ָ��
        struct Prescription
        {
            unsigned int _dataLen;
            const char   *_pData;
        };
        bool send_Prescription(const Prescription &prescription);

		// ������Ƶ���˷���ʹ��ר�õ�socketͨ�������������������н��У�
		struct Video
		{
			unsigned int _dataLen;
			const char   *_pData;
		};
		bool send_Video(const Video &videoPac);

        // ������Ƶ���˷���ʹ��ר�õ�socketͨ�������������������н��У�
        struct Audio
        {
            unsigned int _dataLen;
            const char   *_pData;
        };
        bool send_Audio(const Audio &audioPac);

        // �ǼǴ���ҩ����
        struct DealOfPrescriptionDrug
        {
            unsigned int  _updateDealId;   // ����Ϊ0��ʾinsert�µļ�¼������Ϊ�޸ļ�¼
			std::string   _buyerName;
            unsigned int  _buyerAge;
            bool          _buyerIsMale;
            std::string   _drug_huohao;
            std::string   _drug_mingcheng;
            std::string   _drug_pihao;
            std::string   _drug_guige;
            std::string   _drug_jiliang;
            std::string   _drug_shengchanchangjia;
            std::string   _drug_chufanglaiyuan;
        };
        bool req_AddDealOfPrescriptionDrug(const DealOfPrescriptionDrug &deal);
        bool req_AddDealOfPrescriptionDrug_Sync(const DealOfPrescriptionDrug &deal);
        
        // ��ѯ����ҩ����
        // storeId:    ҩ��Id
        // startTime:  ��ʼʱ�䣨��ʽΪYYYY-MM-DD hh:mm:ss�����մ���ʾ����
        // endTime:    ����ʱ�䣨��ʽΪYYYY-MM-DD hh:mm:ss�����մ���ʾ����
        bool req_ListDealsOfPrescriptionDrug(unsigned int storeId, 
                                             const std::string &startTime,
                                             const std::string &endTime);
        bool req_ListDealsOfPrescriptionDrug_Sync(unsigned int storeId, 
                                                  const std::string &startTime,
                                                  const std::string &endTime);

        // �Ǽ�����ҩ����
        struct DealOfSpecialDrug
        {
            unsigned int  _updateDealId;   // ����Ϊ0��ʾinsert�µļ�¼������Ϊ�޸ļ�¼
            std::string   _buyerName;
            std::string   _buyerShenFenZheng;
            unsigned int  _buyerAge;
            bool          _buyerIsMale;
            std::string   _drug_huohao;
            std::string   _drug_mingcheng;
            std::string   _drug_pihao;
            std::string   _drug_guige;
            std::string   _drug_jiliang;
            std::string   _drug_goumaishuliang;
            std::string   _drug_shengchanchangjia;
            std::string   _drug_chufanglaiyuan;
        };
        bool req_AddDealOfSpecialDrug(const DealOfSpecialDrug &deal);
        bool req_AddDealOfSpecialDrug_Sync(const DealOfSpecialDrug &deal);

        // ��ѯ����ҩ����
        // storeId:    ҩ��Id
        // startTime:  ��ʼʱ�䣨��ʽΪYYYY-MM-DD hh:mm:ss�����մ���ʾ����
        // endTime:    ����ʱ�䣨��ʽΪYYYY-MM-DD hh:mm:ss�����մ���ʾ����
        bool req_ListDealsOfSpecialDrug(unsigned int storeId, 
                                        const std::string &startTime,
                                        const std::string &endTime);
        bool req_ListDealsOfSpecialDrug_Sync(unsigned int storeId, 
                                             const std::string &startTime,
                                             const std::string &endTime);

        // ��ѯԶ����ѯ���̵���ϸ�б�
        // storeId:    ҩ��Id
        // startTime:  ��ʼʱ�䣨��ʽΪYYYY-MM-DD hh:mm:ss�����մ���ʾ����
        // endTime:    ����ʱ�䣨��ʽΪYYYY-MM-DD hh:mm:ss�����մ���ʾ����
        bool req_ListConsultingDetails(unsigned int storeId, 
                                       const std::string &startTime,
                                       const std::string &endTime);
        bool req_ListConsultingDetails_Sync(unsigned int storeId, 
                                            const std::string &startTime,
                                            const std::string &endTime);

		// ��ѯҩʦ���¼
		// pharmacistId: ҩʦId
		// startTime:  ��ʼʱ�䣨��ʽΪYYYY-MM-DD hh:mm:ss�����մ���ʾ����
		// endTime:    ����ʱ�䣨��ʽΪYYYY-MM-DD hh:mm:ss�����մ���ʾ����
		bool req_ListPharmacistActivity(unsigned int pharmacistId, 
			                            const std::string &startTime,
			                            const std::string &endTime);
		bool req_ListPharmacistActivity_Sync(unsigned int pharmacistId, 
			                                 const std::string &startTime,
			                                 const std::string &endTime);

		// ҩ��ʦ����ҩ���ն��˳���ǰ�Ի�
        bool stopConverstaion();

		// ҩ��ʦ����ҩ���ն˷���������Ϣ
		bool send_HB();

		// ҩ��ʦ����ҩ���ն��˳�ϵͳ
        bool logout();
        
    private:
        UserImpl *_pImpl;
    };

} // namespace client