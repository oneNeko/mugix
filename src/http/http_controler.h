#ifndef HTTP_CONTROLER_H
#define HTTP_CONTROLER_H

#include "http_header.h"
#include <string>
using std::string;

namespace mugix
{
    // http控制类
    class HttpControler
    {
    private:
    public:
        // HttpHeader header_;
        string response_header;
        string url_;
        int n_bytes_file_already_sent = 0;
    };
} // namespace mugix

#endif