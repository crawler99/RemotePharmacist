#pragma once

#include "Poco/Util/XMLConfiguration.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/ServerSocket.h"
#include "DatabaseManager.h"
#include "WorkerManager.h"

namespace sserver { namespace server { 
        
    class SessionServer
    {
    public:
        virtual ~SessionServer();

        bool init(Poco::Util::XMLConfiguration *pConfig);
        bool start();

    private:
		std::string serverInfo();
        
        // use the global XML config file
        Poco::Util::XMLConfiguration *_pConfig;
        // server socket to receive client connections
        Poco::SharedPtr<Poco::Net::ServerSocket> _pSSocket;
        // database manager
        Poco::SharedPtr<sserver::server::DatabaseManager> _pDBM;
        // worker manager
        Poco::SharedPtr<sserver::server::WorkerManager> _pWM;
    };

} }