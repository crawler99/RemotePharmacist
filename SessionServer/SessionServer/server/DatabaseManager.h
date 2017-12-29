#pragma once

#include "Poco/Util/XMLConfiguration.h"
#include "Poco/ObjectPool.h"
#include "Poco/Mutex.h"
#include "libpq-fe.h"
#include "Enums.h"
#include "DBCmd.h"
#include "../logger/Logger.h"
#include <sstream>

// =====================================================================================================
struct ConnContext 
{
    std::string _hostIP;
    int _hostPort;
    std::string _dbName;
    std::string _connAcct;
    std::string _connPass;
    std::string _connEncoding;
    int _connTimeout;

    std::string connString()
    {
        std::stringstream ss;
        ss << "hostaddr=" << _hostIP 
            << " port=" << _hostPort 
            << " dbname=" << _dbName 
            << " user=" << _connAcct 
            << " password=" << _connPass 
            << " connect_timeout=" << _connTimeout;
        return ss.str();
    }
};

// =====================================================================================================
template<>
class Poco::PoolableObjectFactory<PGconn>
{
public:
    static void init(const ConnContext &connCtx)
    {
        _connCtx = connCtx;
    }

    PGconn* createObject()
    {
        std::string connStr(_connCtx.connString());
        LOG_INFO("Connecting to database: " + connStr);
        PGconn* pconn = PQconnectdb(connStr.c_str());

        ConnStatusType status = PQstatus(pconn);
        if (CONNECTION_OK == status)
        {
            LOG_INFO("DB connection established");
            PQsetClientEncoding(pconn, _connCtx._connEncoding.c_str());
        }
        else
        {
            LOG_INFO("DB connection failed");
        }
        return pconn;
    }

	bool validateObject(PGconn *pConn)
	{
        ConnStatusType status = PQstatus(pConn);
        if (CONNECTION_OK != status)
        {
            LOG_INFO("DB connection is lost when being returned to pool");
            return false;
        }
        return true;
	}

    void activateObject(PGconn*) {}
    void deactivateObject(PGconn*) {}
    void destroyObject(PGconn*) {}

private:
    static ConnContext _connCtx;
};

// =====================================================================================================
namespace sserver { namespace server { 

    class DatabaseManager
    {
    public:
        virtual ~DatabaseManager();

		virtual bool init(Poco::Util::XMLConfiguration *pConfig, std::string serverInfo);
        virtual bool syncExec(DBCmd &cmd);

    private:
		void reportServerInfo(std::string serverInfo);

        // use the global XML config file
        Poco::Util::XMLConfiguration *_pConfig;
        // connection pool
        Poco::ObjectPool<PGconn> *_pConnPool;
        // lock
        Poco::FastMutex _mutex;
    };

} }