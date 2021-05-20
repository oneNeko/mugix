#include <cstdlib>

#include "config/config.h"
#include "server/http_server.h"

int main(int argc, char *argv[])
{
	// 命令行解析
	auto instance = Config::get_instance();
	instance->parse_config(argc, argv);

	HttpServer server;

	// 初始化
	if (server.Start(instance->PORT))
	{
		server.Run();
	}
	return 0;
}