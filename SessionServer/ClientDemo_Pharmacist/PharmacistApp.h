#pragma once

#include "Pharmacist.h"
#include "../ClientAPI/UserCallback.h"
#include <sstream>
#include <iostream>
#include <fstream>

class PharmacistApp : public Poco::Runnable, public client::UserCallback
{
public:
    // ============================================================================
    // Callback function
    // ============================================================================
    void onLoginAccpted(const UserInfo &info) 
    {
        std::cout << "Login accepted: username=" << info._username
                  << ", userId=" << info._userId
			      << ", userRole=" << info._userRole
                  << ", regTime=" << info._regTime << std::endl;

        if (info._fpLen != 0)
        {
            std::cout << "Has fingerprint" << std::endl;
            std::stringstream ss;
            ss << info._userId << ".fp";
            std::string outFile = ss.str();
            std::ofstream ofs(outFile.c_str(), std::ios::out | std::ios::binary);
            ofs.write(info._pFingerprint, info._fpLen);
            ofs.close();
        }

        if (info._signatureLen != 0)
        {
            std::cout << "Has signature" << std::endl;
            std::stringstream ss;
            ss << info._userId << ".sig";
            std::string outFile = ss.str();
            std::ofstream ofs(outFile.c_str(), std::ios::out | std::ios::binary);
            ofs.write(info._pSignature, info._signatureLen);
            ofs.close();
        }

		_userId = info._userId;
    }

    void onLoginRejected(const std::string &username)
    {
        std::cout << "Login rejected: username=" << username << std::endl;
    }

	void onPharmacistsDetail(const std::vector<client::UserCallback::PharmacistDetailItem> &info)
	{
		std::cout << info.size() << " self detail retrieved" << std::endl;

		std::vector<client::UserCallback::PharmacistDetailItem>::const_iterator iter = info.begin();

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
	}

    void onPatientConnected(unsigned int patientSessionId) 
    {
        std::cout << std::endl;
        std::cout << "Get into conversation: patientSessionId=" << patientSessionId << std::endl;

        _pharmacist->startVideoReceiver();
        _pharmacist->startAudioReceiver();

        // send out the handshake video pac to establish video conversation
        client::User::Video v;
        v._dataLen = 0;
        v._pData = NULL;
        _pharmacist->send_Video(v);

        // send out the handshake audio pac to establish audio conversation
        client::User::Audio a;
        a._dataLen = 0;
        a._pData = NULL;
        _pharmacist->send_Audio(a);
        
        std::cout << "Enter command: ";
    }

    void onPrescription(const client::User::Prescription &prescription) 
    {
        std::cout << std::endl;
        std::cout << "Prescription from patient received: len=" << prescription._dataLen << std::endl;
        std::cout << "Enter command: ";
        
        std::string prescription_back("This a prescription back from pharmacist !!!");
        client::User::Prescription p2;
        p2._dataLen = prescription_back.length();
        p2._pData = prescription_back.c_str();
        if (!_pharmacist->send_Prescription(p2))
        {
            std::cout << "failed to send prescription" << std::endl;
        }
    }

    void onVideo(const client::User::Video &video)
    {
        std::cout << "Receive video from patient: len=" << video._dataLen << std::endl;
    }

    void onAudio(const client::User::Audio &audio)
    {
        std::cout << "Receive audio from patient: len=" << audio._dataLen << std::endl;
    }

    void onPatientQuitService(unsigned int patientSessionId)
    {
        std::cout << std::endl;
        std::cout << "Patient quits conversation: patientSessionId=" << patientSessionId << std::endl;

        _pharmacist->stopVideoReceiver();
        _pharmacist->stopAudioReceiver();
        
        std::cout << "Enter command: ";
    }

    // ============================================================================
    // ============================================================================
    void run()
    {
        std::string helpStr(
            "\n" \
            "command: \n" \
            "  login                    : 登录到服务器 \n" \
			"  refresh_fingerprint      : 重刷指纹 \n" \
			"  self_info                : 拿到自身信息（特别是签名） \n" \
            "  send_video               : 发送视频包（内部循环20次，每次间隔1秒，每次发送一个小字符串） \n" \
            "  send_audio               : 发送音频包（内部循环20次，每次间隔1秒，每次发送一个小字符串） \n" \
            "  stop_conversation        : 退出当前对话 \n" \
            "  logout                   : 退出登录 \n" \
            "  quit                     : 结束程序 \n" \
            "\n"
            );

        std::string command;
        do 
        {
            std::cout << "Enter command: ";
            getline(std::cin, command);

            if (command == "login")
            {
                std::cout << "username: ";
                std::string username;
                getline(std::cin, username);

                std::cout << "password: ";
                std::string password;
                getline(std::cin, password);

                _pharmacist = new client::Pharmacist(username, password, this);
                if (!_pharmacist->connect("127.0.0.1", 31257))
                {
                    std::cout << "Failed to connect to the session server" << std::endl;
                    continue;
                }
                _pharmacist->start();
                _pharmacist->req_Login_Sync(NULL, 0);
            }
			else if (command == "refresh_fingerprint")
			{
				_pharmacist->send_RefreshFingerprint();
			}
			else if (command == "self_info")
			{
				_pharmacist->req_ListPharmacistsDetail_Sync(_userId);
			}
            else if (command == "send_video")
            {
                std::string videoPac("Here is a video pac from pharmacist");
                for (unsigned int i = 0; i < 200; ++i)
                {
                    client::User::Video v;
                    v._dataLen = videoPac.length();
                    v._pData = videoPac.c_str();
                    if (!_pharmacist->send_Video(v))
                    {
                        std::cout << "failed to send video to patient" << std::endl;
                    }
                }
            }
            else if (command == "send_audio")
            {
                std::string audioPac("Here is a audio pac from pharmacist");
                for (unsigned int i = 0; i < 20; ++i)
                {
                    client::User::Audio a;
                    a._dataLen = audioPac.length();
                    a._pData = audioPac.c_str();
                    if (!_pharmacist->send_Audio(a))
                    {
                        std::cout << "failed to send audio to patient" << std::endl;
                    }
                    Poco::Thread::sleep(1000);
                }
            }
            else if (command == "stop_conversation")
            {
                _pharmacist->stopConverstaion();
            }
            else if (command == "logout")
            {
                _pharmacist->logout();
            }
            else if (command == "quit")
            {
                break;
            }
            else if (command == "help")
            {
                std::cout << helpStr << std::endl;
            }
            else
            {
                std::cout << "Un-recognized command, pls try again" << std::endl;
            }
        } while (true);
    }

    void start()
    {
        _thread.start(*this);
    }

    void waitForQuit()
    {
        _thread.join();
    }

private:
    Poco::Thread               _thread;
    client::Pharmacist         *_pharmacist;
	Poco::UInt32               _userId;
};