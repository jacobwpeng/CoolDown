#include "tracker.h"
#include "netpack.h"
#include "tracker_connection_handler.h"

#include <iostream>
#include <exception>
#include "Poco/Logger.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/SQLite/Connector.h"


using Poco::Logger;
using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ReadableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;
using Poco::NObserver;
using Poco::AutoPtr;
using Poco::Thread;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Data::Session;

using std::exception;

Tracker::Tracker(): _helpRequested(false)
	{
	}
	
Tracker::~Tracker()
	{
	}

void Tracker::initialize(Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
    logger().setLevel("debug");
}
		
void Tracker::uninitialize()
{
    ServerApplication::uninitialize();
}

void Tracker::defineOptions(OptionSet& options)
{
    ServerApplication::defineOptions(options);

    options.addOption(
        Option("help", "h", "display help information on command line arguments")
            .required(false)
            .repeatable(false));
}

void Tracker::handleOption(const std::string& name, const std::string& value)
{
    poco_notice_f2(logger(), "option : %s=%s", name, value);
    ServerApplication::handleOption(name, value);

    if (name == "help")
        _helpRequested = true;
}

void Tracker::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("An echo server implemented using the Reactor and Acceptor patterns.");
    helpFormatter.format(std::cout);
}

int Tracker::main(const std::vector<std::string>& args)
{
    try{
        if (_helpRequested)
        {
            displayHelp();
        }
        else
        {
            if( init_db_tables() ){
                return Application::EXIT_CANTCREAT;
            }

            unsigned short port = (unsigned short) config().getInt("EchoServer.port", 9977);

            ServerSocket svs(port);
            SocketReactor reactor;
            SocketAcceptor<TrackerConnectionHandler> acceptor(svs, reactor);

            Thread thread;
            thread.start(reactor);

            waitForTerminationRequest();

            reactor.stop();
            thread.join();
        }
    }catch(exception& e){
        std::cout << e.what() << std::endl;
    }
    return Application::EXIT_OK;
}

//maybe not thread-safe?
int Tracker::init_db_tables(){

    using namespace Poco::Data;
    int ret = 0;

    try{
        Poco::Data::SQLite::Connector::registerConnector();

        string dbFilename = config().getString("Database.Filename", "tracker.db");
        string nodeInfoTableName = config().getString("Database.NodeInfo.TableName", "NodeInfo");
        string fileOwnerTableName = config().getString("Database.FileOwner.TableName", "FileOwner");

        pSession = new Session("SQLite", dbFilename);
        if( NULL == pSession ){
            throw std::runtime_error("Cannot make new session!");
        }
        Session& session = *pSession;
        session << "CREATE TABLE IF NOT EXISTS " << nodeInfoTableName << "("
                   "NodeId VARCHAR(40) PRIMARY KEY,"
                   "LastLoginIp VARCHAR(40),"
                   "MessagePort INTEGER,"
                   "IsOnline INTEGER"
                   ");", now;

        session << "CREATE TABLE IF NOT EXISTS " << fileOwnerTableName << "("
                   "id integer PRIMARY KEY autoincrement,"
                   "FileId VARCHAR(40),"
                   "NodeId VARCHAR(40)"
                   ");", now;

    }catch(exception& e){
        logger().error( e.what() );
        ret = -1;
    }

    return ret;
}
