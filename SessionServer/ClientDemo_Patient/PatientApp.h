#pragma once

#include "Patient.h"
#include "../ClientAPI/UserCallback.h"
#include "../SessionServer/util/Utils.h"
#include <sstream>
#include <iostream>
#include <fstream>

class PatientApp : public Poco::Runnable, public client::UserCallback
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
    }

    void onLoginRejected(const std::string &username)
    {
        std::cout << "Login rejected: username=" << username << std::endl;
    }

    void onPharmacistsDetailOfCurStore(const std::vector<client::UserCallback::PharmacistOfCurrentStore> &detail)
    {
        std::vector<client::UserCallback::PharmacistOfCurrentStore>::const_iterator it = detail.begin();
        while (it != detail.end())
        {
            std::cout << "RealName=" << it->_realName << std::endl;
            std::cout << "CertTypeId=" << it->_certTypeId << std::endl;
            
            if (it->_introductionLen != 0)
            {
                std::cout << "Has intro: " << std::string(it->_pIntroduction, it->_introductionLen) << std::endl;
            }

            if (it->_photoLen != 0)
            {
                std::stringstream ss;
                ss << it->_realName << ".jpg";
                std::string outFile = ss.str();
                std::ofstream ofs(outFile.c_str(), std::ios::out | std::ios::binary);
                ofs.write(it->_pPhoto, it->_photoLen);
                ofs.close();
            }
            ++it;
        }
    }

    void onPharmacistRequestSucc(unsigned int pharmacistSessionId)
    {
        std::cout << "Get into conversation: pharmacistSessionId=" << pharmacistSessionId << std::endl;

        _patient->startVideoReceiver();
        _patient->startAudioReceiver();

        // send out the handshake video pac to establish video conversation
        client::User::Video v;
        v._dataLen = 0;
        v._pData = NULL;
        _patient->send_Video(v);

        // send out the handshake audio pac to establish audio conversation
        client::User::Audio a;
        a._dataLen = 0;
        a._pData = NULL;
        _patient->send_Audio(a);
    }

    void onPharmacistRequestFail()
    {
        std::cout << "Failed to find a pharmacist for service" << std::endl;
    }

    void onPrescription(const client::User::Prescription &prescription) 
    {
        std::cout << std::endl;
        std::cout << "Prescription from pharmacist received: len=" << prescription._dataLen << std::endl;
        std::cout << "Enter command: ";
    }

    void onVideo(const client::User::Video &video)
    {
        std::cout << "Receive video from pharmacist: len=" << video._dataLen << std::endl;
    }

    void onAudio(const client::User::Audio &audio)
    {
        std::cout << "Receive audio from pharmacist: len=" << audio._dataLen << std::endl;
    }

    void onDealOfPrescriptionDrugAdded()
    {
        std::cout << "Successfully add or update a deal of prescription drug" << std::endl;
    }

    void onDealOfPrescriptionDrugAddFail() 
    {
        std::cout << "Failed to add or update a deal of prescription drug" << std::endl;
    }

    void onDealsOfPrescriptionDrugDetail(const std::vector<client::UserCallback::DealsOfPrescriptionDrugDetailItem> &info) 
    {
        std::vector<client::UserCallback::DealsOfPrescriptionDrugDetailItem>::const_iterator cit = info.begin();
        while (cit != info.end())
        {
            std::cout << "Deal id: " << (*cit)._dealId << "\n"
                << "Buyer name: " << (*cit)._buyerName << "\n"
                << "Buyer age: " << (*cit)._buyerAge << "\n"
                << "Buyer is male: " << (*cit)._buyerIsMale << "\n"
                << "Time: " << (*cit)._time << "\n"
                << "Drug HuoHao: " << (*cit)._drugHuoHao << "\n"
                << "Drug MingCheng: " << (*cit)._drugMingCheng << "\n"
                << "Drug PiHao: " << (*cit)._drugPiHao << "\n"
                << "Drug GuiGe: " << (*cit)._drugGuiGe << "\n"
                << "Drug JiLiang: " << (*cit)._drugJiLiang << "\n"
                << "Drug ShengChanChangJia: " << (*cit)._drugShengChanChangJia << "\n"
                << "Drug ChuFangLaiYuan: " << (*cit)._drugChuFangLaiYuan << "\n";
            ++cit;
        }
    }

    void onDealOfSpecialDrugAdded()
    {
        std::cout << "Successfully add or update a deal of special drug" << std::endl;
    }

    void onDealOfSpecialDrugAddFail() 
    {
        std::cout << "Failed to add or update a deal of special drug" << std::endl;
    }

    void onDealsOfSpecialDrugDetail(const std::vector<client::UserCallback::DealsOfSpecialDrugDetailItem> &info) 
    {
        std::vector<client::UserCallback::DealsOfSpecialDrugDetailItem>::const_iterator cit = info.begin();
        while (cit != info.end())
        {
            std::cout << "Deal id: " << (*cit)._dealId << "\n"
                << "Buyer name: " << (*cit)._buyerName << "\n"
                << "Buyer shenfenzheng: " << (*cit)._buyerShenFenZheng << "\n"
                << "Buyer age: " << (*cit)._buyerAge << "\n"
                << "Buyer is male: " << (*cit)._buyerIsMale << "\n"
                << "Time: " << (*cit)._time << "\n"
                << "Drug HuoHao: " << (*cit)._drugHuoHao << "\n"
                << "Drug MingCheng: " << (*cit)._drugMingCheng << "\n"
                << "Drug PiHao: " << (*cit)._drugPiHao << "\n"
                << "Drug GuiGe: " << (*cit)._drugGuiGe << "\n"
                << "Drug JiLiang: " << (*cit)._drugJiLiang << "\n"
                << "Drug GouMaiShuLiang: " << (*cit)._drugGouMaiShuLiang << "\n"
                << "Drug ShengChanChangJia: " << (*cit)._drugShengChanChangJia << "\n"
                << "Drug ChuFangLaiYuan: " << (*cit)._drugChuFangLaiYuan << "\n";
            ++cit;
        }
    }

	void onConsultingDetails(const std::vector<client::UserCallback::ConsultingDetailItem> &info) 
	{
		std::vector<client::UserCallback::ConsultingDetailItem>::const_iterator cit = info.begin();
		while (cit != info.end())
		{
			std::cout << "Pharmacist id: " << (*cit)._pharmacistId << "\n"
				      << "Patient id: " << (*cit)._patientId << "\n"
				      << "Start time: " << (*cit)._startTime << "\n"
					  << "End time: " << (*cit)._endTime << "\n"
                      << "Pharmacist video loc: " << (*cit)._pharmacistVideoLoc << "\n"
                      << "Patient video loc: " << (*cit)._patientVideoLoc << "\n"
                      << "Pharmacist audio loc: " << (*cit)._pharmacistAudioLoc << "\n"
                      << "Patient audio loc: " << (*cit)._patientAudioLoc << "\n"
					  << "Prescription locs: ";
			
			std::vector<std::string>::const_iterator ciit = (*cit)._prescriptionLocs.begin();
			while (ciit != (*cit)._prescriptionLocs.end())
			{
				std::cout << *ciit << ",";
				++ciit;
			}
			std::cout << std::endl;
			++cit;
		}
	}

	void onPharmacistActivityList(const std::vector<std::string> &info) 
	{
		std::vector<std::string>::const_iterator iter = info.begin();
		while (iter != info.end())
		{
			std::cout << "Pharmacist activity: " << *iter << std::endl;
			++iter;
		}
	}

    void onPharmacistQuitService(unsigned int pharmacistSessionId)
    {
        std::cout << std::endl;
        std::cout << "Pharmacist quits conversation: pharmacistSessionId=" << pharmacistSessionId << std::endl;

        _patient->stopVideoReceiver();
        _patient->stopAudioReceiver();

        std::cout << "Enter command: ";
    }

    // ============================================================================
    // ============================================================================
    void run()
    {
        std::string helpStr(
            "\n" \
            "command: \n" \
            "  login                        : 登录到服务器 \n" \
            "  get_pharmacist_info          : 得到当前药店的责任药剂师的信息 \n" \
            "  fetch_pharmacist             : 请求连接当前药店的责任药剂师 \n" \
            "  fetch_global_pharmacist      : 在全局空闲药剂师中寻找一个服务 \n" \
            "  send_prescription            : 发送处方单（内部模拟的字符串） \n" \
            "  send_video                   : 发送视频包（内部循环20次，每次间隔1秒，每次发送一个小字符串） \n" \
            "  send_audio                   : 发送音频包（内部循环20次，每次间隔1秒，每次发送一个小字符串） \n" \
            "  add_prescription_drug_deal   : 登记一笔处方药销售记录 \n" \
            "  list_prescription_drug_deals : 查询处方药销售记录 \n" \
			"  add_special_drug_deal        : 登记一笔特殊药销售记录 \n" \
            "  list_special_drug_deals      : 查询特殊药销售记录 \n" \
            "  list_consulting_details      : 查询咨询记录 \n" \
			"  list_pharmacist_activity     : 查询药师活动记录 \n" \
            "  stop_conversation            : 退出当前对话 \n" \
			"  send_hb                      : 发送心跳包 \n" \
            "  logout                       : 退出登录 \n" \
            "  quit                         : 结束程序 \n" \
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

                _patient = new client::Patient(username, password, this);
                if (!_patient->connect("127.0.0.1", 31257))
                {
                    std::cout << "Failed to connect to the session server" << std::endl;
                    continue;
                }
                _patient->start();
                _patient->req_Login_Sync(NULL, 0);
            }
            else if (command == "get_pharmacist_info")
            {
                _patient->req_PharmacistsDetailOfCurStore_Sync();
            }
            else if (command == "fetch_pharmacist")
            {
				unsigned int certTypeId;
				std::string valStr;
				do 
				{
					std::cout << "certTypeId: ";
					getline(std::cin, valStr);
				} while (!sserver::Utils::string2Num<unsigned int>(valStr, certTypeId));

				_patient->req_Pharmacist_Sync(certTypeId, false);
            }
            else if (command == "fetch_global_pharmacist")
            {
				unsigned int certTypeId;
				std::string valStr;
				do 
				{
					std::cout << "certTypeId: ";
					getline(std::cin, valStr);
				} while (!sserver::Utils::string2Num<unsigned int>(valStr, certTypeId));
				
				_patient->req_Pharmacist_Sync(certTypeId, true);
            }
            else if (command == "send_prescription")
            {
                std::string prescription_go("This a prescription from patient !!!");
                client::User::Prescription p1;
                p1._dataLen = prescription_go.length();
                p1._pData = prescription_go.c_str();
                if (!_patient->send_Prescription(p1))
                {
                    std::cout << "failed to send prescription" << std::endl;
                }
            }
            else if (command == "send_video")
            {
                std::string videoPac("Here is a video pac from patient");
                for (unsigned int i = 0; i < 200; ++i)
                {
                    client::User::Video v;
                    v._dataLen = videoPac.length();
                    v._pData = videoPac.c_str();
                    if (!_patient->send_Video(v))
                    {
                        std::cout << "failed to send video to pharmacist" << std::endl;
                    }
                }
            }
            else if (command == "send_audio")
            {
                std::string audioPac("Here is a audio pac from patient");
                for (unsigned int i = 0; i < 20; ++i)
                {
                    client::User::Audio a;
                    a._dataLen = audioPac.length();
                    a._pData = audioPac.c_str();
                    if (!_patient->send_Audio(a))
                    {
                        std::cout << "failed to send audio to pharmacist" << std::endl;
                    }
                    Poco::Thread::sleep(1000);
                }
            }
            else if (command == "add_prescription_drug_deal")
            {
                client::User::DealOfPrescriptionDrug deal;
                deal._updateDealId=0;
                deal._buyerName="JiangTao";
                deal._buyerAge=32;
                deal._buyerIsMale=true;
                deal._drug_guige="规格";
                deal._drug_huohao="12345678";
                deal._drug_jiliang="1升";
                deal._drug_chufanglaiyuan="科创药业";
                deal._drug_mingcheng="999感冒灵";
                deal._drug_pihao="88888888";
                deal._drug_shengchanchangjia="哈药集团";
                _patient->req_AddDealOfPrescriptionDrug_Sync(deal);
                Poco::Thread::sleep(1000);

                deal._buyerName="LiLing";
                deal._updateDealId=1;
                _patient->req_AddDealOfPrescriptionDrug_Sync(deal);
            }
            else if (command == "list_prescription_drug_deals")
            {
                std::cout << "startTime(YYYY-MM-DD hh:mm:ss): ";
                std::string startTime;
                getline(std::cin, startTime);

                std::cout << "endTime(YYYY-MM-DD hh:mm:ss): ";
                std::string endTime;
                getline(std::cin, endTime);

                _patient->req_ListDealsOfPrescriptionDrug_Sync(1, startTime, endTime);                
            }
            else if (command == "add_special_drug_deal")
            {
                client::User::DealOfSpecialDrug deal;
                deal._updateDealId=0;
                deal._buyerName="JiangTao";
                deal._buyerShenFenZheng="510108";
                deal._buyerAge=32;
                deal._buyerIsMale=true;
                deal._drug_guige="规格";
                deal._drug_huohao="12345678";
                deal._drug_jiliang="1升";
                deal._drug_goumaishuliang="2份";
                deal._drug_chufanglaiyuan="科创药业";
                deal._drug_mingcheng="999感冒灵";
                deal._drug_pihao="88888888";
                deal._drug_shengchanchangjia="哈药集团";
                _patient->req_AddDealOfSpecialDrug_Sync(deal);
                Poco::Thread::sleep(1000);

                deal._buyerName="LiLing";
                deal._updateDealId=1;
                _patient->req_AddDealOfSpecialDrug_Sync(deal);
            }
            else if (command == "list_special_drug_deals")
            {
                std::cout << "startTime(YYYY-MM-DD hh:mm:ss): ";
                std::string startTime;
                getline(std::cin, startTime);

                std::cout << "endTime(YYYY-MM-DD hh:mm:ss): ";
                std::string endTime;
                getline(std::cin, endTime);

                _patient->req_ListDealsOfSpecialDrug_Sync(1, startTime, endTime);
            }
            else if (command == "list_consulting_details")
            {
                std::cout << "startTime(YYYY-MM-DD hh:mm:ss): ";
                std::string startTime;
                getline(std::cin, startTime);

                std::cout << "endTime(YYYY-MM-DD hh:mm:ss): ";
                std::string endTime;
                getline(std::cin, endTime);

                _patient->req_ListConsultingDetails_Sync(1, startTime, endTime);
            }
			else if (command == "list_pharmacist_activity")
			{
				std::cout << "startTime(YYYY-MM-DD hh:mm:ss): ";
				std::string startTime;
				getline(std::cin, startTime);

				std::cout << "endTime(YYYY-MM-DD hh:mm:ss): ";
				std::string endTime;
				getline(std::cin, endTime);

				_patient->req_ListPharmacistActivity_Sync(4, startTime, endTime);
			}
            else if (command == "stop_conversation")
            {
                _patient->stopConverstaion();
            }
            else if (command == "logout")
            {
                _patient->logout();
            }
			else if (command == "send_hb")
			{
				_patient->send_HB();
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
                std::cout << "Un-recognized command, pls try again ..." << std::endl;
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
    Poco::Thread            _thread;
    client::Patient         *_patient;
};