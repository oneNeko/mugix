#include "../utils/utils.h"
#include "../log/log.h"
#include "routes.h"
#include "../config/config.h"

std::string ROUTES::process_requests(std::string request_text)
{
    int pos = int(request_text.find_first_of("\r\n\r\n"));
    // 非法请求
    if (pos == -1)
    {
        return "invaild request";
    }

    auto str_request_header = request_text.substr(0, pos);
    auto str_request_body = request_text.substr(pos + 4);

    // 处理请求行
    HTTP_REQUEST_HEADER header;
    int res = parse_request_header(str_request_header, header);

    if (header.abs_path == "/")
    {
        return make_response("index.html");
    }
    else if (header.abs_path == "/test")
    {
        return make_response(200, "test");
    }

    return make_response(404, "404");
}

int ROUTES::parse_request_header(std::string str_request_header, HTTP_REQUEST_HEADER &header)
{
    // 按行切割请求头
    auto header_lines = SplitString(str_request_header, "\r\n");
    if (header_lines.size() <= 0)
    {
        Log("request header error!");
        return -1;
    }

    //处理请求头的请求行
    auto header_first_line = SplitString(header_lines[0], " ");
    if (header_first_line.size() < 3)
    {
        Log("invaild request header");
        return -1;
    }

    std::string str_request_type = header_first_line[0];
    if (str_request_type == "GET")
    {
        header.http_type = GET;

        // 解析uri路径和参数
        if (int(header_first_line[1].find_first_of('?') == -1))
        {
            header.abs_path = header_first_line[1];
        }
        else
        {
            header.abs_path = header_first_line[1].substr(0, header_first_line[1].find_first_of('?'));
            auto params = SplitString(header_first_line[1].substr(header_first_line[1].find_first_of('?') + 1), "&");
            if (params.size() > 1)
            {
                for (auto iter : params)
                {
                    auto tmp = SplitString(iter, "=");
                    if (tmp.size() >= 2)
                    {
                        header.uri_params[tmp[0]] = tmp[1];
                    }
                }
            }
        }
        header.http_protocol = header_first_line[2];
    }
    else if (str_request_type == "HEAD")
    {
    }
    else if (str_request_type == "POST")
    {
    }

    // 处理请求头的其他行
    for (int i = 1; i < header_lines.size(); i++)
    {
        auto params = SplitString(header_lines[i], ": ");
        if (params.size() != 2)
        {
            continue;
        }
        else
        {
            header.header_params[params[0]] = params[1];
        }
    }

    return 0;
}

std::string ROUTES::get_str_http_status(int status_code)
{
    auto str_status = "200 OK";
    switch (status_code)
    {
    case 200:
    {
        str_status = "200 OK";
        break;
    }
    default:
    {
        str_status = "404 NOT FOUND";
        break;
    }
    }
    return str_status;
}

std::string ROUTES::make_response(int status_code, std::string message)
{

    std::string rmsg = "";
    rmsg = "HTTP/1.1 " + get_str_http_status(status_code) + "\r\n";
    rmsg += "Server: neko_server/0.1\r\n";
    rmsg += "Content-Type: text/html;charset=utf-8\r\n";
    rmsg += "Content-Length: ";
    rmsg += std::to_string(message.size());
    rmsg += "\r\n\r\n";
    rmsg += message;
    return rmsg;
}

std::string ROUTES::make_response(std::string file_path)
{
    auto instance = Config::get_instance();
    auto path = instance->DIR_PATH + file_path;

    Log(path);

    if (IsFileExists(path))
    {
        if (IsFileRead(path))
        {
            
            return make_response(200, ReadFile(path));
        }
        else
        {
            return make_response(403, "403");
        }
    }
    else
    {
        return make_response(404, "404");
    }
}
