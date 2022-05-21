#include "server/core/socket_controler.h"
#include "server/core/logger.h"

int main(int argc, char *argv[])
{
	Logger::getLogger().setLogLevel(Logger::LALL);
	mugix::server::SockerControler sc;
	sc.run();

	return 0;
}