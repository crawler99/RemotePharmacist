#include "logger/Logger.h"
#include "server/SessionServer.h"

int main(int argc , char *argv[])
{
    // read global config file
    const std::string configFile("config/server_config.xml");
    Poco::Util::XMLConfiguration *pConfig = new Poco::Util::XMLConfiguration(configFile);

    // initialize global logger
    if (!sserver::Logger::init(argv[0], pConfig)) return -1;

    // create a session server
    sserver::server::SessionServer server;
    if (!server.init(pConfig)) return -1;
    
    // start the server
    server.start();

    return 0;
}