#include "server/core/socket_controler.h"

int main(int argc, char *argv[])
{
	mugix::server::SockerControler sc;
	sc.run();

	return 0;
}