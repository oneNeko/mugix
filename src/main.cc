#include <unistd.h>

#include "base/config.h"
#include "base/socket_controler.h"
#include "utils/logger.h"
#include "utils/utils.h"

int main(int argc, char *argv[])
{
	Logger::getLogger().setFileName("mugix.log");
	info("mugix start ...");

#ifdef DEBUG
	debug("mugix debug mode on")
	Logger::getLogger().setLogLevel(Logger::LDEBUG);
#else
	Logger::getLogger().setLogLevel(Logger::LINFO);
	daemon(1, 1);
#endif

	mugix::server::SocketControler sc(Config::GetConfig().ip_.c_str(), Config::GetConfig().port_);
	sc.run();

	error("mugix 已退出");

	return 0;
}