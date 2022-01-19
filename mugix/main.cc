#include "server/core/config.h"
#include "server/core/http_server.h"
#include "utils/log.h"

int main(int argc, char *argv[])
{
	// 命令行解析
	auto config = Config::GetInstance();
	config->ParseConfig(argc, argv);

	// 日志
	auto logger=Log::GetInstance();
	if(!logger->Init(config->log_setting_,"./build/log/")){
		printf("log error");
		return 0;
	}

	logger->info("mugix start !");

	HttpServer server;

	server.Init();

	server.EventListen();

	server.EventLoop();

	logger->info("mugix exit !");

	return 0;
}