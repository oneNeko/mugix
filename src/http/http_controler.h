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
        //HttpHeader header_;
        string response_header;
        string url_;
    };
} // namespace mugix

#endif