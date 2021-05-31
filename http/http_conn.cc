#include <assert.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "http_conn.h"
#include "../utils/utils.h"
#include "../routes/routes.h"
#include "../log/log.h"

using namespace std;

int HttpConn::user_count_ = 0;
int HttpConn::epollfd_ = -1;

void HttpConn::Init(int sock_fd, const sockaddr_in &addr)
{
    client_sockfd_ = sock_fd;
    client_address_ = addr;

    Init();
}

void HttpConn::Init()
{
}

void HttpConn::ResetConn(bool real_close)
{
    request_text_ = "";
    request_.Clear();
    response_.Clear();
}

HTTP_CODE HttpConn::ProcessRead()
{
    int pos = request_text_.find("\r\n\r\n");

    if (pos < 0)
    {
        return BAD_REQUEST;
    }
    string header = request_text_.substr(0, pos);

    HTTP_CODE res = request_.ParseHeader(header);

    return res;
}

bool HttpConn::ProcessWrite(HTTP_CODE ret)
{
    ROUTES route;
    route.ProcessRequest(this);
    response_.Process();

    string str_header = response_.GetHeader();

    int n_write = -1;

    if (response_.type_ == T_CONTENT)
    {
        string buf = str_header + response_.content_;
        n_write = write(client_sockfd_, buf.c_str(), buf.size());
    }
    else if (response_.type_ == T_FILE)
    {
        std::ifstream infile;
        std::stringstream buffer;

        infile.open(response_.file_path_);

        buffer << infile.rdbuf();
        std::string contents(buffer.str());

        infile.close();

        string buf = str_header + contents;
        n_write = write(client_sockfd_, buf.c_str(), buf.size());
        //assert(n_write > 0);
    }

    if (n_write < 0)
    {
        printf("socket=%d,n_write=%d\n", client_sockfd_, n_write);
        Log("client close,errno=EINTR");
        Utils::DeleteEvent(epollfd_, client_sockfd_, EPOLLIN);
        close(client_sockfd_);
        //assert(n_write > 0);
    }
    else
    {
        Log("reqponse_: OK");
        Log(str_header);
        //Utils::ModifyFd(epollfd_, client_sockfd, EPOLLIN);
    }
    ResetConn();
    Utils::DeleteEvent(epollfd_, client_sockfd_, EPOLLIN);
    close(client_sockfd_);

    return true;
}

// 处理请求
void HttpConn::Process()
{
    HTTP_CODE read_ret = ProcessRead();
    if (read_ret == NO_REQUEST)
    {
        Utils::ModifyEvent(epollfd_, client_sockfd_, EPOLLIN);
        return;
    }
    bool write_ret = ProcessWrite(read_ret);
    if (!write_ret)
    {
        ResetConn();
    }
    //Utils::ModifyFd(epollfd_, client_sockfd, EPOLLOUT);
}