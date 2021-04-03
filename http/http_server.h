#include <string>
#include <sys/socket.h>

class HttpServer
{
public:
    HttpServer();
    ~HttpServer();

    bool Start(int port);
    void Run();
    void Stop();

private:
    int server_listen_socket;
    int server_port;
    bool is_run;

    bool Init();

    //处理请求
    void ChildSocketProcess(int socket_fd);

    std::string ProcessResponse(std::string response);
    std::string ProcessRequest();
};