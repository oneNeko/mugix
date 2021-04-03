#include <cstdlib>

#include "config.h"
#include "http/http_server.h"

int main(int argc, char *argv[])
{
	// 命令行解析
	Config config;
	config.parse_config(argc, argv);

	HttpServer server;

	// 初始化
	server.Start(config.PORT);
	server.Run();

	return 0;
}