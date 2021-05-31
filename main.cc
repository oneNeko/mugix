#include "config/config.h"
#include "server/http_server.h"

int main(int argc, char *argv[])
{
	// 命令行解析
	auto instance = Config::GetInstance();
	instance->ParseConfig(argc, argv);

	HttpServer server;

	server.Init();

	server.EventListen();

	server.EventLoop();

	return 0;
}