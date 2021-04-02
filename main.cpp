#include "http_server.h"
#include <cstdlib>

int main()
{
	unsigned short port = 50001;
	HttpServer server;
	server.Start(port);
	server.Run();
	return 0;
}
//nohup ./httpserver &