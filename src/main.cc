#include <unistd.h>

#include "base/config.h"
#include "base/socket_controler.h"
#include "utils/logger.h"


int main(int argc, char *argv[])
{
	Logger::getLogger().setLogLevel(Logger::LALL);
	Logger::getLogger().setFileName("mugix.log");

	info("mugix start ...");

	daemon(1, 1);

	mugix::server::SockerControler sc(Config::GetConfig().ip_.c_str(), Config::GetConfig().port_);
	sc.run();

	error("mugix 已退出");

	return 0;
}