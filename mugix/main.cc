#include "server/core/core.h"

int main(int argc, char *argv[])
{
	mugix::server::Core core;
	core.run();

	return 0;
}