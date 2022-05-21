#include <unistd.h>

#include "server/core/socket_controler.h"
#include "server/core/logger.h"

int main(int argc, char *argv[])
{
	Logger::getLogger().setLogLevel(Logger::LALL);
	Logger::getLogger().setFileName("mugix.log");

	info("mugix start ...");

	daemon(1, 1);

	mugix::server::SockerControler sc;
	sc.run();

	error("mugix 已退出");

	return 0;
}