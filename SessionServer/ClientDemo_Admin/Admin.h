#pragma once

#include "../ClientAPI/User.h"
#include "../ClientAPI/UserCallback.h"
#include "Poco/Thread.h"
#include <iostream>
#include <sstream>
#include <fstream>

namespace client {

	class Admin : public User, public UserCallback, public Poco::Runnable
	{
	public:
		Admin(const std::string &username, const std::string &password) 
            : User(username, password, this)
		{
		}

		void run()
		{
			if (!connect("127.0.0.1", 31257))
			{
				std::cout << "Failed to connect to the session server" << std::endl;
			}
			startReceiving();
		}

		void start()
		{
			_thread.start(*this);
		}

		void onLoginAccpted(const UserInfo &info) 
		{
            std::cout << "Login accepted: username=" << info._username
				      << ", userId=" << info._userId
                      << ", userRole=" << info._userRole
                      << ", regTime=" << info._regTime << std::endl;
		}

		void onLoginRejected(const std::string &username)
		{
			std::cout << "Login rejected: username=" << username << std::endl;          
		}

        void onPharmacistCertTypeAdded(const std::string &certTypeName)
        {
            std::cout << "Pharmacist cert type added: " << certTypeName << std::endl;
        }

        void onPharmacistCertTypeAddFail(const std::string &certTypeName)
        {
            std::cout << "Failed to add pharmacist cert type: " << certTypeName << std::endl;
        }

        void onPharmacistCertTypes(const std::vector<PharmacistCertTypeItem> &types)
        {
            std::cout << "List of pharmacist cert types:" << std::endl;
            std::vector<PharmacistCertTypeItem>::const_iterator iter = types.begin();
            for (; iter != types.end(); ++iter)
            {
                std::cout << "id=" << iter->_id << std::endl;
                std::cout << "certTypeName=" << iter->_certTypeName << std::endl;
            }
        }

		void onStoreAdded(const std::string &name, unsigned int storeId) 
		{
			std::cout << "Store added: name=" << name 
                << ", id=" << storeId << std::endl;
		}

		void onStoreAddFail(const std::string &name) 
		{
			std::cout << "Failed to add a store: name=" << name << std::endl;
		}

		void onStoresDeleted()
		{
			std::cout << "Stores deleted" << std::endl;
		}

		void onStoresDeleteFail()
		{
			std::cout << "Failed to delete specified stores" << std::endl;
		}

		void onStores(const std::vector<StoreItem> &stores)
		{
			std::cout << "Store list:" << std::endl;
			std::vector<StoreItem>::const_iterator iter = stores.begin();
			for (; iter != stores.end(); ++iter)
			{
				std::cout << "id=" << iter->_id << std::endl;
				std::cout << "name=" << iter->_name << std::endl;
				std::cout << "addr=" << iter->_addr << std::endl;

                std::set<std::string>::const_iterator iiter = iter->_patientAccts.begin();
                while (iiter != iter->_patientAccts.end())
                {
                    std::cout << "account: " << *iiter << std::endl;
                    ++iiter;
                }
			}
		}

		void onPharmacistAdded(const std::string &username) 
		{
			std::cout << "Pharmacist added: name=" << username << std::endl;
		}

		void onPharmacistAddFail(const std::string &username) 
		{
			std::cout << "Failed to add pharmacist: name=" << username << std::endl;
		}

        void onPharmacistsDeleted()
        {
            std::cout << "Pharmacists deleted" << std::endl;
        }

        void onPharmacistsDeleteFail()
        {
            std::cout << "Failed to delete specified pharmacists" << std::endl;
        }

		void onPharmacistsBrief(const std::vector<PharmacistBriefItem> &info) 
		{
			std::cout << info.size() << " pharmacists retrieved" << std::endl;

			std::vector<PharmacistBriefItem>::const_iterator iter = info.begin();
			while (iter != info.end())
			{
				std::cout << "RealName=" << iter->_realName << std::endl;
                std::cout << "CertTypeId=" << iter->_certTypeId << std::endl;

                if (iter->_photoLen != 0)
                {
                    std::cout << "Has photo" << std::endl;
                    std::stringstream ss;
				    ss << iter->_realName << ".jpg";
				    std::string outFile = ss.str();
				    std::ofstream ofs(outFile.c_str(), std::ios::out | std::ios::binary);
                    ofs.write(iter->_pPhoto, iter->_photoLen);
				    ofs.close();
                }

				++iter;
			}
		}

        void onPharmacistsDetail(const std::vector<PharmacistDetailItem> &info) 
        {
            std::cout << info.size() << " pharmacists detail retrieved" << std::endl;

            std::vector<PharmacistDetailItem>::const_iterator iter = info.begin();
            while (iter != info.end())
            {
                std::cout << "UserName=" << iter->_username << std::endl;
                std::cout << "RealName=" << iter->_realName << std::endl;
                std::cout << "CertTypeId=" << iter->_certTypeId << std::endl;
                std::cout << "ShenFenZheng=" << iter->_shenfenzheng << std::endl;

                if (iter->_certLen != 0)
                {
                    std::cout << "Has cert" << std::endl;
                    std::stringstream ss;
                    ss << iter->_realName << ".cert";
                    std::string outFile = ss.str();
                    std::ofstream ofs(outFile.c_str(), std::ios::out | std::ios::binary);
                    ofs.write(iter->_pCert, iter->_certLen);
                    ofs.close();
                }

                if (iter->_fpLen != 0)
                {
                    std::cout << "Has fingerprint" << std::endl;
                    std::stringstream ss;
                    ss << iter->_realName << ".fp";
                    std::string outFile = ss.str();
                    std::ofstream ofs(outFile.c_str(), std::ios::out | std::ios::binary);
                    ofs.write(iter->_pFingerprint, iter->_fpLen);
                    ofs.close();
                }

                if (iter->_signatureLen != 0)
                {
                    std::cout << "Has signature" << std::endl;
                    std::stringstream ss;
                    ss << iter->_realName << ".sig";
                    std::string outFile = ss.str();
                    std::ofstream ofs(outFile.c_str(), std::ios::out | std::ios::binary);
                    ofs.write(iter->_pSignature, iter->_signatureLen);
                    ofs.close();
                }

				std::cout << "Covered stores: ";
				std::set<unsigned int>::const_iterator iit = iter->_coveredStoreIds.begin();
				while (iit != iter->_coveredStoreIds.end())
				{
					std::cout << *iit << " ";
					++iit;
				}
				std::cout << std::endl;

                if (iter->_introductionLen != 0)
                {
                    std::cout << "Has intro: " << std::string(iter->_pIntroduction, iter->_introductionLen) << std::endl;
                }

                if (iter->_photoLen != 0)
                {
                    std::cout << "Has photo" << std::endl;
                    std::stringstream ss;
                    ss << iter->_realName << ".jpg";
                    std::string outFile = ss.str();
                    std::ofstream ofs(outFile.c_str(), std::ios::out | std::ios::binary);
                    ofs.write(iter->_pPhoto, iter->_photoLen);
                    ofs.close();
                }

                ++iter;
            }
        }

		void onPatientAdded(const std::string &username)
		{
			std::cout << "Patient account added: name=" << username << std::endl;
		}

		void onPatientAddFail(const std::string &username) 
		{
			std::cout << "Failed to add patient account: name=" << username << std::endl;
		}

	private:
		Poco::Thread   _thread;
	};

}