#include "SessionServer.h"
#include "../logger/Logger.h"
#include "DBCmd_ResetPharmacistsStatus.h"
#include "Poco/Exception.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/NetworkInterface.h"
#include "Poco/Net/DNS.h"

namespace sserver { namespace server {

    SessionServer::~SessionServer()
    {          
    }

    bool SessionServer::init(Poco::Util::XMLConfiguration *pConfig)
    {
        if (NULL == pConfig) return false;
        _pConfig = pConfig;

        // create and initialize the database manager
        _pDBM.assign(new sserver::server::DatabaseManager());
        if (!_pDBM->init(_pConfig, serverInfo()))
        {
            LOG_FATAL("Failed to initialize the database connections");
            return false;
        }
		
		DBCmd_ResetPharmacistsStatus cmd;
		if (!_pDBM->syncExec(cmd))
		{
			LOG_FATAL("Failed to reset the status of all pharmacists");
			return false;
		}

        // create and initialize the worker manager
        _pWM.assign(new sserver::server::WorkerManager(_pDBM));
        if (!_pWM->init(_pConfig))
        {
            LOG_FATAL("Failed to initialize the worker threads");
            return false;
        }

        // initialize server socket
        try
        {
            int listenPort = _pConfig->getInt("ListenPort");
            int backlog = _pConfig->getInt("ConnectionBacklog");
            _pSSocket = Poco::SharedPtr<Poco::Net::ServerSocket>(new Poco::Net::ServerSocket(listenPort, backlog));
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

        return true;
    }

    bool SessionServer::start()
    {
        while (true)
        {
            Poco::Net::StreamSocket sock = _pSSocket->acceptConnection();
            _pWM->assignClient(sock);
        }
        return true;
    }

	std::string SessionServer::serverInfo()
    {
        std::ostringstream os;
        if (_pConfig != NULL)
        {
            _pConfig->save(os);
        }

        std::string info =  "Host name: " + Poco::Net::DNS::hostName() + "\r\n";
        info += "IP addresses: ";
        Poco::Net::NetworkInterface::NetworkInterfaceList nics = Poco::Net::NetworkInterface::list();
        Poco::Net::NetworkInterface::NetworkInterfaceList::const_iterator it = nics.begin();
        while (it != nics.end())
        {
            info += it->address().toString() + ", ";
            ++it;
        }
        info += "\r\n";
        info += "Server version: " + SERVER_VERSION + "\r\n";
        info += os.str();

		return info;
    }

} }
