#include "WorkerManager.h"
#include "SessionIdGenerator.h"
#include "../message/ClientMessages.h"
#include "../util/Utils.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include <sstream>

namespace sserver { namespace server {

    WorkerManager::WorkerManager(Poco::SharedPtr<sserver::server::DatabaseManager> pDBM)
    {
        _pDBM = pDBM;
        _nextWorkerIdx = 0;
        _numOfIdlePharmacists = 0;
        _numOfBusyPharmacists = 0;
    }

    WorkerManager::~WorkerManager()
    {
		_clientHBTimer.stop();
		_pPrescriptionSaver->stop();
        _pVideoSaver->stop();
		_pAudioSaver->stop();
    }

    bool WorkerManager::init(Poco::Util::XMLConfiguration *pConfig)
    {
        if (NULL == pConfig) return false;
        _pConfig = pConfig;

        try
        {
            // 1. create and start worker threads
			_workerNum = _pConfig->getInt("WorkingThreads[@size]");
			std::stringstream ss;
			ss << "Creating " << _workerNum << " working threads ...";
			LOG_INFO(ss.str());

            for (unsigned int i = 0; i < _workerNum; ++i)
            {
                Poco::SharedPtr<Worker> pWorker(new Worker(_pDBM, this));
                _workerList.push_back(pWorker);
                pWorker->start();
            }
            LOG_INFO("Working threads created successfully");

			// 2. deal with client heartbeat
			_clientHBTimeoutSec = _pConfig->getInt("ClientHeartbeat[@timeout]");
			_checkPharmacistHB = (_pConfig->getString("ClientHeartbeat[@check_pharmacist]") == "true");
			_checkPatientHB = (_pConfig->getString("ClientHeartbeat[@check_patient]") == "true");
			ss.str("");
			ss << "Client heartbeat timeout: " << _clientHBTimeoutSec << " seconds, check ["
				<< (_checkPharmacistHB ? "pharmacist " : "") 
				<< (_checkPatientHB ? "patient" : "") << "]";
			LOG_INFO(ss.str());
			_clientHBTimer.setStartInterval(MILLISECONDS_PER_SECOND * _clientHBTimeoutSec);
			_clientHBTimer.setPeriodicInterval(MILLISECONDS_PER_SECOND * _clientHBTimeoutSec);
			_clientHBTimer.start(Poco::TimerCallback<WorkerManager>(*this, &WorkerManager::_onClientHBTimer));

			// 3. create directory for saving prescriptions, 
            //    and start the prescription saver
            Poco::Path path1(pConfig->getString("FileStore.Prescriptions"));
            _prescriptionDir = path1.toString();
            if (_prescriptionDir.empty())
            {
                LOG_FATAL("Empty configuration item: FileStore.Prescriptions");
                return false;
            }

			Poco::File f1(_prescriptionDir);
			f1.createDirectories();
            _pPrescriptionSaver.assign(new SimpleFileSaver(_prescriptionDir));
            _pPrescriptionSaver->start();

            // 4. create directory for saving videos, 
            //    and start the video saver
            Poco::Path path2(pConfig->getString("FileStore.Videos"));
            _videoDir = path2.toString();
            if (_videoDir.empty())
            {
                LOG_FATAL("Empty configuration item: FileStore.Videos");
                return false;
            }

            Poco::File f2(_videoDir);
            f2.createDirectories();
            _pVideoSaver.assign(new ContinuousFileSaver(_videoDir));
            _pVideoSaver->start();

			// 5. create directory for saving audios, 
			//    and start the audio saver
			Poco::Path path3(pConfig->getString("FileStore.Audios"));
			_audioDir = path3.toString();
			if (_audioDir.empty())
			{
				LOG_FATAL("Empty configuration item: FileStore.Audios");
				return false;
			}

			Poco::File f3(_audioDir);
			f3.createDirectories();
			_pAudioSaver.assign(new ContinuousFileSaver(_audioDir));
			_pAudioSaver->start();
        }
        catch (Poco::NotFoundException &e)
        {
            LOG_FATAL("Missing configuration item: " + e.message());
            return false;
        }
        catch (Poco::SyntaxException &e)
        {
            LOG_FATAL("Syntax error when reading configuration: " + e.message());
            return false;
        }
		catch (...)
		{
			LOG_FATAL("Error when initializing worker manager");
			return false;
		}

        return true;
    }

    void WorkerManager::assignClient(Poco::Net::StreamSocket sock)
    {
        _workerList[_nextWorkerIdx++]->assignClient(sock);
        _nextWorkerIdx = (_nextWorkerIdx + 1) % _workerNum;
    }

	void WorkerManager::onPharmacistIn(Poco::UInt32 sessionId, const PharmacistConn &pharmacistConn)
    {
		std::stringstream ss;
		ss << "Pharmacist is in, sessionId=" << sessionId;
		LOG_INFO(ss.str());
		
		Poco::RWLock::ScopedWriteLock locker(_lock);
		_socks2Session[pharmacistConn._sock] = sessionId;
		_pharmacistConns[sessionId] = pharmacistConn;
		_idlePharmacists[pharmacistConn._userId] = pharmacistConn;
		++_numOfIdlePharmacists;

        ss.str("");
        ss << "Idle pharmacists: " << _numOfIdlePharmacists << ", busy pharmacists: " << _numOfBusyPharmacists;
        LOG_INFO(ss.str());
    }

	void WorkerManager::onPatientIn(Poco::UInt32 sessionId, const PatientConn &patientConn)
    {
        std::stringstream ss;
        ss << "Patient is in, sessionId=" << sessionId;
        LOG_INFO(ss.str());

        Poco::RWLock::ScopedWriteLock locker(_lock);
		_socks2Session[patientConn._sock] = sessionId;
        _patientConns[sessionId] = patientConn;
    }

	void WorkerManager::onPharmacistOut(Poco::UInt32 sessionId)
	{
		std::stringstream ss;
		ss << "Pharmacist is out, sessionId=" << sessionId;
		LOG_INFO(ss.str());

		Poco::RWLock::ScopedWriteLock locker(_lock);

		std::map<Poco::UInt32, PharmacistConn>::iterator iter = _pharmacistConns.find(sessionId);
		if (iter != _pharmacistConns.end())
		{
			_idlePharmacists.erase(iter->second._userId);
			_socks2Session.erase(iter->second._sock);
			Utils::closeSocket(iter->second._sock);
			_pharmacistConns.erase(iter);
		}	
		
		// pharmacist must be idle status before logout
		--_numOfIdlePharmacists;

		ss.str("");
		ss << "Idle pharmacists: " << _numOfIdlePharmacists << ", busy pharmacists: " << _numOfBusyPharmacists;
		LOG_INFO(ss.str());

		// clear the HB tracer
		_clientHBExaminees.erase(sessionId);
	}
	
	void WorkerManager::onPatientOut(Poco::UInt32 sessionId)
	{
		std::stringstream ss;
		ss << "Patient is out, sessionId=" << sessionId;
		LOG_INFO(ss.str());

		Poco::RWLock::ScopedWriteLock locker(_lock);

		std::map<Poco::UInt32, PatientConn>::iterator iter = _patientConns.find(sessionId);
		if (iter != _patientConns.end())
		{
			_socks2Session.erase(iter->second._sock);
			Utils::closeSocket(iter->second._sock);
			_patientConns.erase(iter);
		}

		// clear the HB tracer
		_clientHBExaminees.erase(sessionId);
	}

	void WorkerManager::onClientHeartbeat(Poco::UInt32 sessionId, const ClientHBExaminee &examinee)
	{
		Poco::RWLock::ScopedWriteLock locker(_lock);

		if ((SessionIdGenerator::checkRole(sessionId, USER_PHARMACIST) && _checkPharmacistHB) ||
			(SessionIdGenerator::checkRole(sessionId, USER_PATIENT) && _checkPatientHB))
		{
			_clientHBExaminees[sessionId] = examinee;
		}		
	}

	PharmacistConn WorkerManager::onRequestForPharmacist(Poco::Net::StreamSocket sock, Poco::UInt32 sessionId, 
		                                                 const std::set<Poco::UInt32> &pharmacistIds)
    {
		Poco::RWLock::ScopedWriteLock locker(_lock);

		std::map<Poco::UInt32, PharmacistConn>::iterator it1 = _idlePharmacists.end();
		std::set<Poco::UInt32>::const_iterator it2 = pharmacistIds.begin();
		while (it2 != pharmacistIds.end())
		{
			it1 = _idlePharmacists.find(*it2);
			if (it1 != _idlePharmacists.end())
			{
				break;
			}
			++it2;
		}

		if (it1 != _idlePharmacists.end())
		{
			PharmacistConn pharmacistConn = it1->second;

            ConversationEnd_Pharmacist phEnd;
            phEnd._conn = pharmacistConn;
            phEnd._conversationId = 0;
			_patientToPharmacist[sessionId] = phEnd;

            ConversationEnd_Patient paEnd;
            std::map<Poco::UInt32, PatientConn>::iterator iiter = _patientConns.find(sessionId);
            if (iiter != _patientConns.end())
            {
                paEnd._conn = iiter->second;
            }
            paEnd._conversationId = 0;
			_pharmacistToPatient[pharmacistConn._sessionId] = paEnd;
			_idlePharmacists.erase(it1);

			std::stringstream ss;
			ss << "Find the pharmacist (sessionId=" << pharmacistConn._sessionId
				<< ") for patient (sessionId =" << sessionId << ")";
			LOG_INFO(ss.str());

			--_numOfIdlePharmacists;
			++_numOfBusyPharmacists;

			ss.str("");
			ss << "Idle pharmacists: " << _numOfIdlePharmacists << ", busy pharmacists: " << _numOfBusyPharmacists;
			LOG_INFO(ss.str());

			return pharmacistConn;
		}

		PharmacistConn non(0, 0, 0, Poco::Net::StreamSocket());
		return non;
    }

    PharmacistConn WorkerManager::onRequestForPharmacist_UsePool(Poco::Net::StreamSocket sock, 
		                                                         Poco::UInt32 sessionId,
		                                                         Poco::UInt32 certTypeId)
    {
        Poco::RWLock::ScopedWriteLock locker(_lock);

		std::map<Poco::UInt32, PharmacistConn>::iterator iter = _idlePharmacists.begin();
		while (iter != _idlePharmacists.end())
		{
			if (iter->second._certTypeId == certTypeId)
			{
				break;
			}
			++iter;
		}

        if (iter != _idlePharmacists.end())
        {
            PharmacistConn pharmacistConn = iter->second;

            ConversationEnd_Pharmacist phEnd;
            phEnd._conn = pharmacistConn;
            phEnd._conversationId = 0;
            _patientToPharmacist[sessionId] = phEnd;

            ConversationEnd_Patient paEnd;
            std::map<Poco::UInt32, PatientConn>::iterator iiter = _patientConns.find(sessionId);
            if (iiter != _patientConns.end())
            {
                paEnd._conn = iiter->second;
            }
            paEnd._conversationId = 0;
            _pharmacistToPatient[pharmacistConn._sessionId] = paEnd;
            _idlePharmacists.erase(iter);

            std::stringstream ss;
            ss << "Find the pharmacist (sessionId=" << pharmacistConn._sessionId
                << ") for patient (sessionId =" << sessionId << ")";
            LOG_INFO(ss.str());

            --_numOfIdlePharmacists;
            ++_numOfBusyPharmacists;

            ss.str("");
            ss << "Idle pharmacists: " << _numOfIdlePharmacists << ", busy pharmacists: " << _numOfBusyPharmacists;
            LOG_INFO(ss.str());

            return pharmacistConn;
        }

        PharmacistConn non(0, 0, 0, Poco::Net::StreamSocket());
        return non;
    }

	void WorkerManager::fillConversationId(Poco::UInt32 conversationId, Poco::UInt32 pharmacistSessionId, Poco::UInt32 patientSessionId)
	{
		Poco::RWLock::ScopedWriteLock locker(_lock);

		std::map<Poco::UInt32, ConversationEnd_Patient>::iterator iter1 = _pharmacistToPatient.find(pharmacistSessionId);
		if (iter1 != _pharmacistToPatient.end())
		{
			iter1->second._conversationId = conversationId;
		}

		std::map<Poco::UInt32, ConversationEnd_Pharmacist>::iterator iter2 = _patientToPharmacist.find(patientSessionId);
		if (iter2 != _patientToPharmacist.end())
		{
			iter2->second._conversationId = conversationId;
		}
	}

	bool WorkerManager::onPatientQuitConversation(Poco::UInt32 patientSessionId, PharmacistConn &targetPharmacist)
	{
		Poco::RWLock::ScopedWriteLock locker(_lock);

		// close related patient fos
		ContinuousFileSaver::File f1;
		f1._generatorId = patientSessionId;
        f1._type = ContinuousFileSaver::File::CHUNK_FILE_END;
		_pVideoSaver->addFileToSave(f1);
		_pAudioSaver->addFileToSave(f1);

		std::map<Poco::UInt32, ConversationEnd_Pharmacist>::iterator iter = _patientToPharmacist.find(patientSessionId);
        if (iter != _patientToPharmacist.end())
        {
			Poco::UInt32 pharmacistSessionId = iter->second._conn._sessionId;

			// close related pharmacist fos
			ContinuousFileSaver::File f2;
			f2._generatorId = pharmacistSessionId;
			f2._type = ContinuousFileSaver::File::CHUNK_FILE_END;
			_pVideoSaver->addFileToSave(f2);
			_pAudioSaver->addFileToSave(f2);

			//
			std::map<Poco::UInt32, PharmacistConn>::iterator phIter = _pharmacistConns.find(pharmacistSessionId);
			if (phIter != _pharmacistConns.end())
			{
				targetPharmacist = phIter->second;
				_pharmacistToPatient.erase(pharmacistSessionId);
			}
            _patientToPharmacist.erase(iter);

            // remove the audio and video socket map
            {
                Poco::RWLock::ScopedWriteLock locker(_videoLock);
				
				std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator it1
					= _pharmacistToPatientVideo.find(pharmacistSessionId);
				if (it1 != _pharmacistToPatientVideo.end())
				{
					Utils::closeSocket(it1->second.first);
					_pharmacistToPatientVideo.erase(it1);
				}
				
				std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator it2
					= _patientToPharmacistVideo.find(patientSessionId);
				if (it2 != _patientToPharmacistVideo.end())
				{
					Utils::closeSocket(it2->second.first);
					_patientToPharmacistVideo.erase(it2);
				}
            }
            {
                Poco::RWLock::ScopedWriteLock locker(_audioLock);

				std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator it1
					= _pharmacistToPatientAudio.find(pharmacistSessionId);
				if (it1 != _pharmacistToPatientAudio.end())
				{
					Utils::closeSocket(it1->second.first);
					_pharmacistToPatientAudio.erase(it1);
				}

				std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator it2
					= _patientToPharmacistAudio.find(patientSessionId);
				if (it2 != _patientToPharmacistAudio.end())
				{
					Utils::closeSocket(it2->second.first);
					_patientToPharmacistAudio.erase(it2);
				}
            }

            std::stringstream ss;
            ss << "Conversation ended: pharmacist sessionId=" << pharmacistSessionId << ", patient sessionId =" << patientSessionId;
            LOG_INFO(ss.str());

			_idlePharmacists[targetPharmacist._userId] = targetPharmacist;
            --_numOfBusyPharmacists;
            ++_numOfIdlePharmacists;

            ss.str("");
            ss << "Idle pharmacists: " << _numOfIdlePharmacists << ", busy pharmacists: " << _numOfBusyPharmacists;
            LOG_INFO(ss.str());

            return true;
        }
        return false;
	}

	bool WorkerManager::onPharmacistQuitConversation(Poco::UInt32 pharmacistSessionId, PatientConn &targetPatient)
    {
        Poco::RWLock::ScopedWriteLock locker(_lock);

		// close related pharmacist fos
		ContinuousFileSaver::File f1;
		f1._generatorId = pharmacistSessionId;
		f1._type = ContinuousFileSaver::File::CHUNK_FILE_END;
		_pVideoSaver->addFileToSave(f1);
		_pAudioSaver->addFileToSave(f1);

		std::map<Poco::UInt32, ConversationEnd_Patient>::iterator iter = _pharmacistToPatient.find(pharmacistSessionId);
        if (iter != _pharmacistToPatient.end())
        {
			Poco::UInt32 patientSessionId = iter->second._conn._sessionId;

			// close related patient fos
			ContinuousFileSaver::File f2;
			f2._generatorId = patientSessionId;
			f2._type = ContinuousFileSaver::File::CHUNK_FILE_END;
			_pVideoSaver->addFileToSave(f2);
			_pAudioSaver->addFileToSave(f2);

			//
			std::map<Poco::UInt32, PatientConn>::iterator paIter = _patientConns.find(patientSessionId);
			if (paIter != _patientConns.end())
			{
				targetPatient = paIter->second;
				_patientToPharmacist.erase(patientSessionId);
			}
			_pharmacistToPatient.erase(iter);

            // remove the audio and video socket map
            {
                Poco::RWLock::ScopedWriteLock locker(_videoLock);

				std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator it1
					= _pharmacistToPatientVideo.find(pharmacistSessionId);
				if (it1 != _pharmacistToPatientVideo.end())
				{
					Utils::closeSocket(it1->second.first);
					_pharmacistToPatientVideo.erase(it1);
				}

				std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator it2
					= _patientToPharmacistVideo.find(patientSessionId);
				if (it2 != _patientToPharmacistVideo.end())
				{
					Utils::closeSocket(it2->second.first);
					_patientToPharmacistVideo.erase(it2);
				}                
            }
            {
                Poco::RWLock::ScopedWriteLock locker(_audioLock);

				std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator it1
					= _pharmacistToPatientAudio.find(pharmacistSessionId);
				if (it1 != _pharmacistToPatientAudio.end())
				{
					Utils::closeSocket(it1->second.first);
					_pharmacistToPatientAudio.erase(it1);
				}

				std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator it2
					= _patientToPharmacistAudio.find(patientSessionId);
				if (it2 != _patientToPharmacistAudio.end())
				{
					Utils::closeSocket(it2->second.first);
					_patientToPharmacistAudio.erase(it2);
				}
            }

            std::stringstream ss;
            ss << "Conversation ended: pharmacist sessionId=" << pharmacistSessionId << ", patient sessionId =" << patientSessionId;
            LOG_INFO(ss.str());

			std::map<Poco::UInt32, PharmacistConn>::iterator iiter = _pharmacistConns.find(pharmacistSessionId);
			if (iiter != _pharmacistConns.end())
			{
				_idlePharmacists[iiter->second._userId] = iiter->second;
			}
			
			--_numOfBusyPharmacists;
            ++_numOfIdlePharmacists;

            ss.str("");
            ss << "Idle pharmacists: " << _numOfIdlePharmacists << ", busy pharmacists: " << _numOfBusyPharmacists;
            LOG_INFO(ss.str());

            return true;
        }
        return false;
    }

    const PharmacistConn* WorkerManager::getPharmacistConn(Poco::UInt32 pharmacistSessionId)
    {
        Poco::RWLock::ScopedReadLock locker(_lock);

        std::map<Poco::UInt32, PharmacistConn>::iterator iter = _pharmacistConns.find(pharmacistSessionId);
        return (iter != _pharmacistConns.end() ? &(iter->second) : NULL);
    }

    const PatientConn* WorkerManager::getPatientConn(Poco::UInt32 patientSessionId)
    {
        Poco::RWLock::ScopedReadLock locker(_lock);

        std::map<Poco::UInt32, PatientConn>::iterator iter = _patientConns.find(patientSessionId);
        return (iter != _patientConns.end() ? &(iter->second) : NULL);
    }

	Poco::UInt32 WorkerManager::getSessionId(const Poco::Net::StreamSocket &sock)
	{
		Poco::RWLock::ScopedReadLock locker(_lock);

		std::map<Poco::Net::StreamSocket, Poco::UInt32>::iterator iter = _socks2Session.find(sock);
		return (iter != _socks2Session.end() ? iter->second : 0);
	}

	ConversationEnd_Pharmacist* WorkerManager::getTargetPharmacist(Poco::UInt32 patientSessionId)
    {
		Poco::RWLock::ScopedReadLock locker(_lock);

		std::map<Poco::UInt32, ConversationEnd_Pharmacist>::iterator iter = _patientToPharmacist.find(patientSessionId);
		if (iter != _patientToPharmacist.end())
		{
            return &(iter->second);
		}
		return NULL;
    }

	ConversationEnd_Patient* WorkerManager::getTargetPatient(Poco::UInt32 pharmacistSessionId)
    {
        Poco::RWLock::ScopedReadLock locker(_lock);

		std::map<Poco::UInt32, ConversationEnd_Patient>::iterator iter = _pharmacistToPatient.find(pharmacistSessionId);
		if (iter != _pharmacistToPatient.end())
		{
			return &(iter->second);
		}
		return NULL;
    }

    Poco::Net::StreamSocket* WorkerManager::getTargetPharmacistVideoSock(Poco::UInt32 patientSessionId,
                                                                         const Poco::Net::StreamSocket &patientVideoSock)
    {
		Poco::Net::StreamSocket *target = NULL;
		
		{
            // first look up the video socket map
            Poco::RWLock::ScopedReadLock locker(_videoLock);
			std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator iter 
				= _patientToPharmacistVideo.find(patientSessionId);
            if (iter != _patientToPharmacistVideo.end())
            {
				target = &(iter->second.first);
				if (iter->second.second)         // if the you already knows me
				{
					return target;
				}
				else
				{
					iter->second.second = true; // this time I'll expose myself to you
                                                // this statement is thread-safe so doesn't require write lock
				}
            }
        }

        {
            // add the source socket to the video socket map
            Poco::RWLock::ScopedReadLock locker(_lock);
            std::map<Poco::UInt32, ConversationEnd_Pharmacist>::iterator iter = _patientToPharmacist.find(patientSessionId);
            if (iter != _patientToPharmacist.end())
            {
                Poco::RWLock::ScopedWriteLock locker(_videoLock);
				_pharmacistToPatientVideo[iter->second._conn._sessionId] = std::make_pair(patientVideoSock, target != NULL);                
            }
        }

        return target;
    }

    Poco::Net::StreamSocket* WorkerManager::getTargetPatientVideoSock(Poco::UInt32 pharmacistSessionId,
                                                                      const Poco::Net::StreamSocket &pharmacistVideoSock)
    {
		Poco::Net::StreamSocket *target = NULL;

		{
			// first look up the video socket map
			Poco::RWLock::ScopedReadLock locker(_videoLock);
			std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator iter 
				= _pharmacistToPatientVideo.find(pharmacistSessionId);
			if (iter != _pharmacistToPatientVideo.end())
			{
				target = &(iter->second.first);
				if (iter->second.second)         // if the you already knows me
				{
					return target;
				}
				else
				{
					iter->second.second = true; // this time I'll expose myself to you
                                                // this statement is thread-safe so doesn't require write lock
				}
			}
		}

		{
			// add the source socket to the video socket map
			Poco::RWLock::ScopedReadLock locker(_lock);
			std::map<Poco::UInt32, ConversationEnd_Patient>::iterator iter = _pharmacistToPatient.find(pharmacistSessionId);
			if (iter != _pharmacistToPatient.end())
			{
				Poco::RWLock::ScopedWriteLock locker(_videoLock);
				_patientToPharmacistVideo[iter->second._conn._sessionId] = std::make_pair(pharmacistVideoSock, target != NULL);                
			}
		}

		return target;
    }

    Poco::Net::StreamSocket* WorkerManager::getTargetPharmacistAudioSock(Poco::UInt32 patientSessionId,
                                                                         const Poco::Net::StreamSocket &patientAudioSock)
    {
        Poco::Net::StreamSocket *target = NULL;

        {
            // first look up the audio socket map
            Poco::RWLock::ScopedReadLock locker(_audioLock);
            std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator iter 
                = _patientToPharmacistAudio.find(patientSessionId);
            if (iter != _patientToPharmacistAudio.end())
            {
                target = &(iter->second.first);
                if (iter->second.second)         // if the you already knows me
                {
                    return target;
                }
                else
                {
                    iter->second.second = true; // this time I'll expose myself to you
                                                // this statement is thread-safe so doesn't require write lock
                }
            }
        }

        {
            // add the source socket to the audio socket map
            Poco::RWLock::ScopedReadLock locker(_lock);
            std::map<Poco::UInt32, ConversationEnd_Pharmacist>::iterator iter = _patientToPharmacist.find(patientSessionId);
            if (iter != _patientToPharmacist.end())
            {
                Poco::RWLock::ScopedWriteLock locker(_audioLock);
                _pharmacistToPatientAudio[iter->second._conn._sessionId] = std::make_pair(patientAudioSock, target != NULL);                
            }
        }

        return target;
    }

    Poco::Net::StreamSocket* WorkerManager::getTargetPatientAudioSock(Poco::UInt32 pharmacistSessionId,
                                                                      const Poco::Net::StreamSocket &pharmacistAudioSock)
    {
        Poco::Net::StreamSocket *target = NULL;

        {
            // first look up the video socket map
            Poco::RWLock::ScopedReadLock locker(_audioLock);
            std::map<Poco::UInt32, std::pair<Poco::Net::StreamSocket, bool> >::iterator iter 
                = _pharmacistToPatientAudio.find(pharmacistSessionId);
            if (iter != _pharmacistToPatientAudio.end())
            {
                target = &(iter->second.first);
                if (iter->second.second)         // if the you already knows me
                {
                    return target;
                }
                else
                {
                    iter->second.second = true; // this time I'll expose myself to you
                                                // this statement is thread-safe so doesn't require write lock
                }
            }
        }

        {
            // add the source socket to the audio socket map
            Poco::RWLock::ScopedReadLock locker(_lock);
            std::map<Poco::UInt32, ConversationEnd_Patient>::iterator iter = _pharmacistToPatient.find(pharmacistSessionId);
            if (iter != _pharmacistToPatient.end())
            {
                Poco::RWLock::ScopedWriteLock locker(_audioLock);
                _patientToPharmacistAudio[iter->second._conn._sessionId] = std::make_pair(pharmacistAudioSock, target != NULL);                
            }
        }

        return target;
    }

	void WorkerManager::_onClientHBTimer(Poco::Timer &timer)
	{
		std::vector<std::pair<Poco::UInt32, ClientHBExaminee> > toTerminate;

		{
			Poco::RWLock::ScopedWriteLock locker(_lock);

			std::map<Poco::UInt32, ClientHBExaminee>::const_iterator iter = _clientHBExaminees.begin();
			while (iter != _clientHBExaminees.end())
			{
				if (iter->second._timestamp.elapsed() > (MICROSECONDS_PER_SECOND * _clientHBTimeoutSec))
				{
					toTerminate.push_back(std::make_pair(iter->first, iter->second));
					_clientHBExaminees.erase(iter++);
				}
				else
				{
					++iter;
				}
			}
		}

		std::stringstream ss;
		std::vector<std::pair<Poco::UInt32, ClientHBExaminee> >::iterator iter = toTerminate.begin();
		while (iter != toTerminate.end())
		{
			ss.str("");
			ss << "HB lost, sessionId=" << iter->first;
			LOG_WARNING(ss.str());

			sserver::message::LogoutRequest msg;
			msg._header.setPktLen(sizeof(msg));
			msg._header.setSessionId(iter->first);

			iter->second._worker->onLogoutRequest(iter->second._sock, reinterpret_cast<char*>(&msg));
			++iter;
		}
	}

} }