#include "Admin.h"
#include <fstream>

int main()
{
	std::cout << "===================== ����Ա��Ϊ =============================" << std::endl;

	// ��¼���ɹ�
    // -------------------------------------------------------------------------
	client::Admin admin1("Admin1", "12345");
	admin1.start();
	Poco::Thread::sleep(1500);
	admin1.req_Login_Sync(NULL, 0);
	Poco::Thread::sleep(1500);

	// ��¼�ɹ�
    // -------------------------------------------------------------------------
	client::Admin admin("Admin", "12345");
    admin.start();
	Poco::Thread::sleep(1500);
	admin.req_Login_Sync(NULL, 0);
	Poco::Thread::sleep(1500);

    // ���ҩ��ʦ֤������
    // -------------------------------------------------------------------------
    admin.req_AddPharmacistCertType_Sync("��ҩ");
    Poco::Thread::sleep(1000);
    admin.req_AddPharmacistCertType_Sync("ҩѧ");
    Poco::Thread::sleep(1000);

    // �õ�����ҩ��ʦ֤�������б�
    // -------------------------------------------------------------------------
    admin.req_ListPharmacistCertTypes_Sync();
    Poco::Thread::sleep(1500);

	// ���һ��ҩ��ʦ
    // -------------------------------------------------------------------------
    std::string pharmacistName1("zhangsan");
	std::string intro1("��ҵ������ҽ�ƴ�ѧ������20��ḻ�ľ���");
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
    prInfo1._password = "";                // ��ӵ�ʱ����������Ϊ��
	prInfo1._shenfenzheng = "5101081981";
    prInfo1._realName = "����";
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

	// ���һ���ظ���ҩ��ʦ - ʧ��
    // -------------------------------------------------------------------------
    admin.req_AddPharmacist_Sync(prInfo1);
	Poco::Thread::sleep(1500);

    // �޸�һ���Ѵ��ڵ�ҩ��ʦ (ȥ��ͷ��,�޸����֤)
    // -------------------------------------------------------------------------
	prInfo1._updatePharmacistId = 2;
    prInfo1._password = "";                // �޸ĵ�ʱ�򣬿������ʾԭ�����벻��
	prInfo1._photoLen = 0;
	prInfo1._pPhoto = NULL;
	prInfo1._shenfenzheng = "5101081986";
    admin.req_AddPharmacist_Sync(prInfo1);
    Poco::Thread::sleep(1500);

    // ��ӵڶ���ҩ��ʦ
    // -------------------------------------------------------------------------
    std::string pharmacistName2("lisi");
    std::string intro2("��ҵ�ڻ���ҽ�ƴ�ѧ����10��ḻ����");
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
    prInfo2._realName = "����";
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

    // ɾ���ڶ���ҩ��ʦ
    // -------------------------------------------------------------------------
    std::set<unsigned int> dp;
    dp.insert(4);
    admin.req_DeletePharmacists_Sync(dp);
    Poco::Thread::sleep(1500);

    // ������ӵڶ���ҩ��ʦ(�޸���ʵ����������)
    // -------------------------------------------------------------------------
    prInfo2._realName = "���Ķ�";
    prInfo2._password = "54321";

    // ��Ϊphoto�Ѿ���release������������Ҫ�ӻ���
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

	// ��ӵ�����ҩ��ʦ (û�н��ܺ�ͷ��)
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
	prInfo3._realName = "����";
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

    // �޸��Ѵ��ڵĵ�����ҩ��ʦ (�޸�����)
    // -------------------------------------------------------------------------
    prInfo3._updatePharmacistId = 5;
    prInfo3._password = "54321";
    admin.req_AddPharmacist_Sync(prInfo3);
    Poco::Thread::sleep(1500);

	// ���һ��ҩ�꣨��ָ��ҩ��ʦ��
    // -------------------------------------------------------------------------
    client::User::StoreRegInfo srInfo;
    srInfo._updateStoreId = 0;
    srInfo._name = "�ƴ�ҩҵ����¥�ֵ�";
    srInfo._addr = "����¥��25��";
	admin.req_AddStore_Sync(srInfo);
	Poco::Thread::sleep(1500);

	// ����ظ�ҩ�� - ʧ��
    // -------------------------------------------------------------------------
	admin.req_AddStore_Sync(srInfo);
	Poco::Thread::sleep(1500);

    // �޸�һ���Ѵ��ڵ�ҩ�� (�޸ĵ�ַ)
    // -------------------------------------------------------------------------
    srInfo._addr = "����¥��18��";
    srInfo._updateStoreId = 1;
    admin.req_AddStore_Sync(srInfo);
    Poco::Thread::sleep(1500);

	// ��ӵڶ���ҩ�꣨��ָ��ҩ��ʦ��
    // -------------------------------------------------------------------------
	client::User::StoreRegInfo srInfo2;
	srInfo2._updateStoreId = 0;
	srInfo2._name = "�ƴ�ҩҵ������ֵ�";
	srInfo2._addr = "�������1��";
	admin.req_AddStore_Sync(srInfo2);
	Poco::Thread::sleep(1500);

	// ɾ���ڶ���ҩ��
    // -------------------------------------------------------------------------
	std::set<unsigned int> ds;
	ds.insert(3);
	admin.req_DeleteStores_Sync(ds);
	Poco::Thread::sleep(1500);

	// ������ӵڶ���ҩ��(�޸ĵ�ַ)
    // -------------------------------------------------------------------------
	srInfo2._addr="�������100��";
	admin.req_AddStore_Sync(srInfo2);
	Poco::Thread::sleep(1500);

    // ��ӵ�����ҩ�꣨��ָ��ҩ��ʦ��
    // -------------------------------------------------------------------------
    client::User::StoreRegInfo srInfo3;
    srInfo3._updateStoreId = 0;
    srInfo3._name = "�ƴ�ҩҵ����·�ֵ�";
    srInfo3._addr = "����·���ٶ���";
    admin.req_AddStore_Sync(srInfo3);
    Poco::Thread::sleep(1500);

    // �õ�����ҩ��ʦ������Ϣ(��������Ƭ)
    // -------------------------------------------------------------------------
    admin.req_ListPharmacists_Sync();
    Poco::Thread::sleep(1500);

	// �õ�����ҩ��ʦ������Ϣ(������Ƭ)
	// -------------------------------------------------------------------------
	admin.req_ListPharmacists_Sync(true);
	Poco::Thread::sleep(1500);

    // �õ�����ҩ��ʦ��ϸ��Ϣ
    // -------------------------------------------------------------------------
    admin.req_ListPharmacistsDetail_Sync();
    Poco::Thread::sleep(1500);

	// �õ�ĳһ��ҩ��ʦ����ϸ��Ϣ
	// -------------------------------------------------------------------------
	admin.req_ListPharmacistsDetail_Sync(4);
	Poco::Thread::sleep(1500);
	
	// ���һ��ҩ���˺�
    // -------------------------------------------------------------------------
    client::User::StoreAccountRegInfo sarInfo1;
    sarInfo1._isUpdate = false;
    sarInfo1._username = "hpl";
    sarInfo1._password = "12345";
    sarInfo1._storeId = 1;
    admin.req_AddStoreAccount_Sync(sarInfo1);
	Poco::Thread::sleep(1500);

	// �ظ����һ��ҩ���˺� - ʧ��
    // -------------------------------------------------------------------------
	admin.req_AddStoreAccount_Sync(sarInfo1);
	Poco::Thread::sleep(1500);

    // ��ӵڶ���ҩ���˺�
    // -------------------------------------------------------------------------
    client::User::StoreAccountRegInfo sarInfo2;
    sarInfo2._isUpdate = false;
    sarInfo2._username = "hpl2";
    sarInfo2._password = "12345";
    sarInfo2._storeId = 1;
    admin.req_AddStoreAccount_Sync(sarInfo2);
    Poco::Thread::sleep(1500);

    // ��ӵ�����ҩ���˺�
    // -------------------------------------------------------------------------
    client::User::StoreAccountRegInfo sarInfo3;
    sarInfo3._isUpdate = false;
    sarInfo3._username = "jsl";
    sarInfo3._password = "12345";
    sarInfo3._storeId = 4;
    admin.req_AddStoreAccount_Sync(sarInfo3);
    Poco::Thread::sleep(1500);

    // �޸ĵ�����ҩ���˺ŵ�����
    // -------------------------------------------------------------------------
    sarInfo3._isUpdate=true;
    sarInfo3._password="54321";
    admin.req_AddStoreAccount_Sync(sarInfo3);
    Poco::Thread::sleep(1500);

	// ɾ��������ҩ��
    // -------------------------------------------------------------------------
	std::set<unsigned int> dds;
	dds.insert(4);
	admin.req_DeleteStores_Sync(dds);
	Poco::Thread::sleep(1500);

	// ������ӵ�����ҩ��
    // -------------------------------------------------------------------------
	admin.req_AddStore_Sync(srInfo3);
	Poco::Thread::sleep(1500);

	// ������ӵ�����ҩ���˺ţ������룩
    // -------------------------------------------------------------------------
	sarInfo3._isUpdate = false;
	sarInfo3._password = "12345";
	admin.req_AddStoreAccount_Sync(sarInfo3);
	Poco::Thread::sleep(1500);

    // �õ�����ҩ���б�
    // -------------------------------------------------------------------------
    admin.req_ListStores_Sync();
    Poco::Thread::sleep(1500);

	return 0;
}