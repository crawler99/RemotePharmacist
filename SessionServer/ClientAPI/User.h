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
        
		// 连接服务器
        bool connect(const std::string &serverIP, unsigned int serverPort);
		
		// ===============================================================================
		// 阻塞的方法
		// ===============================================================================
		
		// 进入接收消息循环
        void startReceiving();

		// 进入接收视频包循环
		void startReceivingVideo();

		// 进入接收音频包循环
		void startReceivingAudio();

		// ===============================================================================
		// 非阻塞的方法
		// ===============================================================================

        // 停止接收消息
        void stopReceiving();

		// 停止接收视频包
		void stopReceivingVideo();

		// 停止接收音频包
		void stopReceivingAudio();
        
        // 请求登录
		bool req_Login(const char *pFingerprint, unsigned int fpLen);
        bool req_Login_Sync(const char *pFingerprint, unsigned int fpLen);

		// 重刷指纹
		bool send_RefreshFingerprint();

        // 请求添加一个药剂师证书类型（中药/西药/...）
        // certTypeName:       药剂师证书类型名称
        bool req_AddPharmacistCertType(const std::string &certTypeName);
        bool req_AddPharmacistCertType_Sync(const std::string &certTypeName);

        // 请求所有药剂师证书类型列表
        bool req_ListPharmacistCertTypes();
        bool req_ListPharmacistCertTypes_Sync();

		// 请求添加一个药剂师
		// username:           药剂师用户名
		// password:           药剂师密码
		// shenfenzheng:       药师身份证
		// realName:           药剂师真名
        // certTypeId:         药剂师证书类型Id
        // certLen             药剂师证书大小 （必须不为0）
        // pCert               药剂师证书数据指针
        // fpLen               药剂师指纹大小 （必须不为0）
        // pFingerprint        药剂师指纹数据指针
        // signatureLen        药剂师签名大小 （必须不为0）
        // pSignature          药剂师签名数据指针
		// introductionLen:    药剂师介绍文字长度 (如果设置为0，则表示没有介绍)
		// pIntroduction:      药剂师介绍文字指针
		// photoLen:           药剂师照片大小 （如果设置为0，则表示没有照片）
		// pPhoto:             药剂师照片数据指针
        struct PharmacistRegInfo
        {
            unsigned int      _updatePharmacistId;   // 如果为0则表示添加，否则表示要修改的Id
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

        // 请求删除一个或多个药师
        // pharmacistIds:      要删除的药师Id集合
        bool req_DeletePharmacists(const std::set<unsigned int>  &pharmacistIds);
        bool req_DeletePharmacists_Sync(const std::set<unsigned int>  &pharmacistIds);

		// 请求所有药剂师的基本信息
        bool req_ListPharmacists(bool includePhoto = false);
		bool req_ListPharmacists_Sync(bool includePhoto = false);

        // 请求所有药剂师的详细信息
		// userId:             目标药剂师的用Id，如果设置为0，则表示请求全部的药剂师
        bool req_ListPharmacistsDetail(unsigned int userId = 0);
        bool req_ListPharmacistsDetail_Sync(unsigned int userId = 0);

		// 请求添加一个药店
		// name:               药店名称
		// addr:               药店地址
		// pharmacistIds:      所有责任药剂师的Id
        struct StoreRegInfo
        {
            unsigned int            _updateStoreId;    // 如果为0则表示添加，否则表示要修改的Id
            std::string             _name;
            std::string             _addr;
			std::set<unsigned int>  _pharmacistIds;
        };
		bool req_AddStore(const StoreRegInfo &info);
        bool req_AddStore_Sync(const StoreRegInfo &info);

		// 请求删除一个或多个药店
		// storeIds:           要删除的药店Id集合
		bool req_DeleteStores(const std::set<unsigned int>  &storeIds);
		bool req_DeleteStores_Sync(const std::set<unsigned int>  &storeIds);
		
		// 请求所有药店列表
		bool req_ListStores();
        bool req_ListStores_Sync();
        
		// 请求添加一个药店账号
        // isUpdate:  是否为修改请求（只会修改密码）
		// username:  账号名
		// password:  账号密码
		// storeId:   药店ID，从药店列表中选取
        struct StoreAccountRegInfo
        {
            bool         _isUpdate;
            std::string  _username;
            std::string  _password;
            unsigned int _storeId;
        };
		bool req_AddStoreAccount(const StoreAccountRegInfo &info);
        bool req_AddStoreAccount_Sync(const StoreAccountRegInfo &info);

        // 请求当前药店的药剂师信息
        bool req_PharmacistsDetailOfCurStore();
        bool req_PharmacistsDetailOfCurStore_Sync();

		// 终端请求药剂师服务
		// certTypeId: 药剂师证书类型Id
        // usePool:    是否不关心当前药店的责任药师，而直接从全局空闲药师里面寻找
        bool req_Pharmacist(unsigned int certTypeId, bool usePool);
        bool req_Pharmacist_Sync(unsigned int certTypeId, bool usePool);

        // 传输处方单数据
        // dataLen:    处方单数据长度
        // pData:      处方单数据指针
        struct Prescription
        {
            unsigned int _dataLen;
            const char   *_pData;
        };
        bool send_Prescription(const Prescription &prescription);

		// 传输视频（此方法使用专用的socket通道，可以与其他请求并行进行）
		struct Video
		{
			unsigned int _dataLen;
			const char   *_pData;
		};
		bool send_Video(const Video &videoPac);

        // 传输音频（此方法使用专用的socket通道，可以与其他请求并行进行）
        struct Audio
        {
            unsigned int _dataLen;
            const char   *_pData;
        };
        bool send_Audio(const Audio &audioPac);

        // 登记处方药销售
        struct DealOfPrescriptionDrug
        {
            unsigned int  _updateDealId;   // 设置为0表示insert新的纪录，否则为修改记录
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
        
        // 查询处方药销售
        // storeId:    药店Id
        // startTime:  起始时间（格式为YYYY-MM-DD hh:mm:ss），空串表示最早
        // endTime:    结束时间（格式为YYYY-MM-DD hh:mm:ss），空串表示最晚
        bool req_ListDealsOfPrescriptionDrug(unsigned int storeId, 
                                             const std::string &startTime,
                                             const std::string &endTime);
        bool req_ListDealsOfPrescriptionDrug_Sync(unsigned int storeId, 
                                                  const std::string &startTime,
                                                  const std::string &endTime);

        // 登记特殊药销售
        struct DealOfSpecialDrug
        {
            unsigned int  _updateDealId;   // 设置为0表示insert新的纪录，否则为修改记录
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

        // 查询特殊药销售
        // storeId:    药店Id
        // startTime:  起始时间（格式为YYYY-MM-DD hh:mm:ss），空串表示最早
        // endTime:    结束时间（格式为YYYY-MM-DD hh:mm:ss），空串表示最晚
        bool req_ListDealsOfSpecialDrug(unsigned int storeId, 
                                        const std::string &startTime,
                                        const std::string &endTime);
        bool req_ListDealsOfSpecialDrug_Sync(unsigned int storeId, 
                                             const std::string &startTime,
                                             const std::string &endTime);

        // 查询远程咨询过程的详细列表
        // storeId:    药店Id
        // startTime:  起始时间（格式为YYYY-MM-DD hh:mm:ss），空串表示最早
        // endTime:    结束时间（格式为YYYY-MM-DD hh:mm:ss），空串表示最晚
        bool req_ListConsultingDetails(unsigned int storeId, 
                                       const std::string &startTime,
                                       const std::string &endTime);
        bool req_ListConsultingDetails_Sync(unsigned int storeId, 
                                            const std::string &startTime,
                                            const std::string &endTime);

		// 查询药师活动记录
		// pharmacistId: 药师Id
		// startTime:  起始时间（格式为YYYY-MM-DD hh:mm:ss），空串表示最早
		// endTime:    结束时间（格式为YYYY-MM-DD hh:mm:ss），空串表示最晚
		bool req_ListPharmacistActivity(unsigned int pharmacistId, 
			                            const std::string &startTime,
			                            const std::string &endTime);
		bool req_ListPharmacistActivity_Sync(unsigned int pharmacistId, 
			                                 const std::string &startTime,
			                                 const std::string &endTime);

		// 药剂师或者药店终端退出当前对话
        bool stopConverstaion();

		// 药剂师或者药店终端发送心跳消息
		bool send_HB();

		// 药剂师或者药店终端退出系统
        bool logout();
        
    private:
        UserImpl *_pImpl;
    };

} // namespace client