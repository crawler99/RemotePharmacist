#include "Admin.h"
#include <fstream>

int main()
{
	std::cout << "===================== 管理员行为 =============================" << std::endl;

	// 登录不成功
    // -------------------------------------------------------------------------
	client::Admin admin1("Admin1", "12345");
	admin1.start();
	Poco::Thread::sleep(1500);
	admin1.req_Login_Sync(NULL, 0);
	Poco::Thread::sleep(1500);

	// 登录成功
    // -------------------------------------------------------------------------
	client::Admin admin("Admin", "12345");
    admin.start();
	Poco::Thread::sleep(1500);
	admin.req_Login_Sync(NULL, 0);
	Poco::Thread::sleep(1500);

    // 添加药剂师证书类型
    // -------------------------------------------------------------------------
    admin.req_AddPharmacistCertType_Sync("中药");
    Poco::Thread::sleep(1000);
    admin.req_AddPharmacistCertType_Sync("药学");
    Poco::Thread::sleep(1000);

    // 得到所有药剂师证书类型列表
    // -------------------------------------------------------------------------
    admin.req_ListPharmacistCertTypes_Sync();
    Poco::Thread::sleep(1500);

	// 添加一个药剂师
    // -------------------------------------------------------------------------
    std::string pharmacistName1("zhangsan");
	std::string intro1("毕业于重庆医科大学，具有20年丰富的经验");
    std::string cert1("Cert1");
    std::string fingerprint1("FingerPrint1");
    std::string signature1("Signature1");
	std::ifstream photoStream1;
	photoStream1.open("pharmacist_photo.jpg", std::ios::in | std::ios::binary);
	photoStream1.seekg(0, std::ios::end);
	int len1 = photoStream1.tellg();
	char *pPhoto1 = new char[len1];
	photoStream1.seekg(0, 0);
	photoStream1.read(pPhoto1, len1);

    client::User::PharmacistRegInfo prInfo1;
    prInfo1._updatePharmacistId = 0;
    prInfo1._username = pharmacistName1;
    prInfo1._password = "";                // 添加的时候允许密码为空
	prInfo1._shenfenzheng = "5101081981";
    prInfo1._realName = "张三";
    prInfo1._certTypeId = 1;
    prInfo1._certLen = cert1.length();
    prInfo1._pCert = cert1.c_str();
    prInfo1._fpLen = fingerprint1.length();
    prInfo1._pFingerprint = fingerprint1.c_str();
    prInfo1._signatureLen = signature1.length();
    prInfo1._pSignature = signature1.c_str();
    prInfo1._introductionLen = intro1.length();
    prInfo1._pIntroduction = intro1.c_str();
    prInfo1._photoLen = len1;
    prInfo1._pPhoto = pPhoto1;

	admin.req_AddPharmacist_Sync(prInfo1);

	delete [] pPhoto1;
	photoStream1.close();
	Poco::Thread::sleep(1500);

	// 添加一个重复的药剂师 - 失败
    // -------------------------------------------------------------------------
    admin.req_AddPharmacist_Sync(prInfo1);
	Poco::Thread::sleep(1500);

    // 修改一个已存在的药剂师 (去掉头像,修改身份证)
    // -------------------------------------------------------------------------
	prInfo1._updatePharmacistId = 2;
    prInfo1._password = "";                // 修改的时候，空密码表示原来密码不变
	prInfo1._photoLen = 0;
	prInfo1._pPhoto = NULL;
	prInfo1._shenfenzheng = "5101081986";
    admin.req_AddPharmacist_Sync(prInfo1);
    Poco::Thread::sleep(1500);

    // 添加第二个药剂师
    // -------------------------------------------------------------------------
    std::string pharmacistName2("lisi");
    std::string intro2("毕业于华西医科大学，有10年丰富经验");
    std::string cert2("Cert2");
    std::string fingerprint2("FingerPrint2");
    std::string signature2("Signature2");
    std::ifstream photoStream2;
    photoStream2.open("pharmacist_photo.jpg", std::ios::in | std::ios::binary);
    photoStream2.seekg(0, std::ios::end);
    int len2 = photoStream2.tellg();
    char *pPhoto2 = new char[len2];
    photoStream2.seekg(0, 0);
    photoStream2.read(pPhoto2, len2);

    client::User::PharmacistRegInfo prInfo2;
    prInfo2._updatePharmacistId = 0;
    prInfo2._username = pharmacistName2;
    prInfo2._password = "12345";
	prInfo2._shenfenzheng = "510106";
    prInfo2._realName = "李四";
    prInfo2._certTypeId = 1;
    prInfo2._certLen = cert2.length();
    prInfo2._pCert = cert2.c_str();
    prInfo2._fpLen = fingerprint2.length();
    prInfo2._pFingerprint = fingerprint2.c_str();
    prInfo2._signatureLen = signature2.length();
    prInfo2._pSignature = signature2.c_str();
    prInfo2._introductionLen = intro2.length();
    prInfo2._pIntroduction = intro2.c_str();
    prInfo2._photoLen = len2;
    prInfo2._pPhoto = pPhoto2;

    admin.req_AddPharmacist_Sync(prInfo2);

    delete [] pPhoto2;
    photoStream2.close();
    Poco::Thread::sleep(1500);

    // 删除第二个药剂师
    // -------------------------------------------------------------------------
    std::set<unsigned int> dp;
    dp.insert(4);
    admin.req_DeletePharmacists_Sync(dp);
    Poco::Thread::sleep(1500);

    // 重新添加第二个药剂师(修改真实姓名和密码)
    // -------------------------------------------------------------------------
    prInfo2._realName = "李四二";
    prInfo2._password = "54321";

    // 因为photo已经被release掉，所以这里要加回来
    std::ifstream photoStream2_1;
    photoStream2_1.open("pharmacist_photo.jpg", std::ios::in | std::ios::binary);
    photoStream2_1.seekg(0, std::ios::end);
    int len2_1 = photoStream2_1.tellg();
    char *pPhoto2_1 = new char[len2_1];
    photoStream2_1.seekg(0, 0);
    photoStream2_1.read(pPhoto2_1, len2_1);
    prInfo2._photoLen = len2_1;
    prInfo2._pPhoto = pPhoto2_1;

    admin.req_AddPharmacist_Sync(prInfo2);
    Poco::Thread::sleep(1500);

	// 添加第三个药剂师 (没有介绍和头像)
    // -------------------------------------------------------------------------
	std::string pharmacistName3("wangwu");
	std::string cert3("Cert3");
	std::string fingerprint3("FingerPrint3");
    std::string signature3("Signature3");

	client::User::PharmacistRegInfo prInfo3;
    prInfo3._updatePharmacistId = 0;
	prInfo3._username = pharmacistName3;
	prInfo3._password = "12345";
	prInfo3._shenfenzheng = "510107";
	prInfo3._realName = "王五";
	prInfo3._certTypeId = 2;
	prInfo3._certLen = cert3.length();
	prInfo3._pCert = cert3.c_str();
	prInfo3._fpLen = fingerprint3.length();
	prInfo3._pFingerprint = fingerprint3.c_str();
    prInfo3._signatureLen = signature3.length();
    prInfo3._pSignature = signature3.c_str();
	prInfo3._introductionLen = 0;
	prInfo3._photoLen = 0;

	admin.req_AddPharmacist_Sync(prInfo3);
	Poco::Thread::sleep(1500);

    // 修改已存在的第三个药剂师 (修改密码)
    // -------------------------------------------------------------------------
    prInfo3._updatePharmacistId = 5;
    prInfo3._password = "54321";
    admin.req_AddPharmacist_Sync(prInfo3);
    Poco::Thread::sleep(1500);

	// 添加一个药店（不指定药剂师）
    // -------------------------------------------------------------------------
    client::User::StoreRegInfo srInfo;
    srInfo._updateStoreId = 0;
    srInfo._name = "科创药业红牌楼分店";
    srInfo._addr = "红牌楼街25号";
	admin.req_AddStore_Sync(srInfo);
	Poco::Thread::sleep(1500);

	// 添加重复药店 - 失败
    // -------------------------------------------------------------------------
	admin.req_AddStore_Sync(srInfo);
	Poco::Thread::sleep(1500);

    // 修改一个已存在的药店 (修改地址)
    // -------------------------------------------------------------------------
    srInfo._addr = "红牌楼街18号";
    srInfo._updateStoreId = 1;
    admin.req_AddStore_Sync(srInfo);
    Poco::Thread::sleep(1500);

	// 添加第二个药店（不指定药剂师）
    // -------------------------------------------------------------------------
	client::User::StoreRegInfo srInfo2;
	srInfo2._updateStoreId = 0;
	srInfo2._name = "科创药业武侯祠分店";
	srInfo2._addr = "武侯祠街1号";
	admin.req_AddStore_Sync(srInfo2);
	Poco::Thread::sleep(1500);

	// 删除第二个药店
    // -------------------------------------------------------------------------
	std::set<unsigned int> ds;
	ds.insert(3);
	admin.req_DeleteStores_Sync(ds);
	Poco::Thread::sleep(1500);

	// 重新添加第二个药店(修改地址)
    // -------------------------------------------------------------------------
	srInfo2._addr="武侯祠街100号";
	admin.req_AddStore_Sync(srInfo2);
	Poco::Thread::sleep(1500);

    // 添加第三个药店（不指定药剂师）
    // -------------------------------------------------------------------------
    client::User::StoreRegInfo srInfo3;
    srInfo3._updateStoreId = 0;
    srInfo3._name = "科创药业建设路分店";
    srInfo3._addr = "建设路伊藤对面";
    admin.req_AddStore_Sync(srInfo3);
    Poco::Thread::sleep(1500);

    // 得到所有药剂师基本信息(不包含照片)
    // -------------------------------------------------------------------------
    admin.req_ListPharmacists_Sync();
    Poco::Thread::sleep(1500);

	// 得到所有药剂师基本信息(包含照片)
	// -------------------------------------------------------------------------
	admin.req_ListPharmacists_Sync(true);
	Poco::Thread::sleep(1500);

    // 得到所有药剂师详细信息
    // -------------------------------------------------------------------------
    admin.req_ListPharmacistsDetail_Sync();
    Poco::Thread::sleep(1500);

	// 得到某一个药剂师的详细信息
	// -------------------------------------------------------------------------
	admin.req_ListPharmacistsDetail_Sync(4);
	Poco::Thread::sleep(1500);
	
	// 添加一个药店账号
    // -------------------------------------------------------------------------
    client::User::StoreAccountRegInfo sarInfo1;
    sarInfo1._isUpdate = false;
    sarInfo1._username = "hpl";
    sarInfo1._password = "12345";
    sarInfo1._storeId = 1;
    admin.req_AddStoreAccount_Sync(sarInfo1);
	Poco::Thread::sleep(1500);

	// 重复添加一个药店账号 - 失败
    // -------------------------------------------------------------------------
	admin.req_AddStoreAccount_Sync(sarInfo1);
	Poco::Thread::sleep(1500);

    // 添加第二个药店账号
    // -------------------------------------------------------------------------
    client::User::StoreAccountRegInfo sarInfo2;
    sarInfo2._isUpdate = false;
    sarInfo2._username = "hpl2";
    sarInfo2._password = "12345";
    sarInfo2._storeId = 1;
    admin.req_AddStoreAccount_Sync(sarInfo2);
    Poco::Thread::sleep(1500);

    // 添加第三个药店账号
    // -------------------------------------------------------------------------
    client::User::StoreAccountRegInfo sarInfo3;
    sarInfo3._isUpdate = false;
    sarInfo3._username = "jsl";
    sarInfo3._password = "12345";
    sarInfo3._storeId = 4;
    admin.req_AddStoreAccount_Sync(sarInfo3);
    Poco::Thread::sleep(1500);

    // 修改第三个药店账号的密码
    // -------------------------------------------------------------------------
    sarInfo3._isUpdate=true;
    sarInfo3._password="54321";
    admin.req_AddStoreAccount_Sync(sarInfo3);
    Poco::Thread::sleep(1500);

	// 删除第三个药店
    // -------------------------------------------------------------------------
	std::set<unsigned int> dds;
	dds.insert(4);
	admin.req_DeleteStores_Sync(dds);
	Poco::Thread::sleep(1500);

	// 重新添加第三个药店
    // -------------------------------------------------------------------------
	admin.req_AddStore_Sync(srInfo3);
	Poco::Thread::sleep(1500);

	// 重新添加第三个药店账号（新密码）
    // -------------------------------------------------------------------------
	sarInfo3._isUpdate = false;
	sarInfo3._password = "12345";
	admin.req_AddStoreAccount_Sync(sarInfo3);
	Poco::Thread::sleep(1500);

    // 得到所有药店列表
    // -------------------------------------------------------------------------
    admin.req_ListStores_Sync();
    Poco::Thread::sleep(1500);

	return 0;
}