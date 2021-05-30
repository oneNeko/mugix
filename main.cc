#include <cstdlib>

#include "config/config.h"
#include "server/http_server.h"

int main(int argc, char *argv[])
{
	// 命令行解析
	auto instance = Config::get_instance();
	instance->parse_config(argc, argv);

	HttpServer server;

	server.Init();

	server.InitThreadPool();

	server.EventListen();
	server.EventLoop();

	return 0;
}