#include <cstdlib>

#include "config/config.h"
#include "server/http_server.h"

int main(int argc, char *argv[])
{
	// 命令行解析
	Config config;
	config.parse_config(argc, argv);
	
	HttpServer server;

	// 初始化
	if(server.Start(config.PORT)){
		server.Run();
	}
	return 0;
}