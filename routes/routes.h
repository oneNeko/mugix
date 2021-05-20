#ifndef ROUTES_H
#define ROUTES_H
#endif

#include <set>
#include <string>
#include "../utils/define.h"

class ROUTES
{
private:
    int parse_request_header(std::string request_header, HTTP_REQUEST_HEADER &header);
    std::string make_response(int status_code, std::string message);
    std::string get_str_http_status(int status_code);

public:
    std::string process_requests(std::string request_text);
};