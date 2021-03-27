#include "http_server.h"

int main(){
    unsigned short port = 50001;
	HttpServer server;
	server.Start(port);
	server.Run();

	getchar();
    return 0;
}
//nohup ./httpserver &