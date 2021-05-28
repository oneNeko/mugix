#include "../utils/utils.h"
#include "../log/log.h"
#include "routes.h"
#include "../config/config.h"

std::string ROUTES::login(std::string post_data)
{
    auto params = SplitString(post_data, "&");
    std::map<SS> data;
    for (auto iter : params)
    {
        auto it = SplitString(iter, "=");
        data[it[0]] = it[1];
    }
    auto str = UrlDecode(post_data);
    Log(str);
    if (data["password"] == "123456")
    {
        return make_response("login success", "/", 302);
    }
    return make_response("login.html", HTTP_RESPONSE_HEADER(), TYPE_FILE);
}

std::string ROUTES::process_requests(std::string request_text)
{
    Log(request_text);

    int pos = int(request_text.find_first_of("\r\n\r\n"));
    // 非法请求
    if (pos == -1)
    {
        Log("Bad Request");
        return make_response("Bad Request", 400);
    }

    auto str_request_header = request_text.substr(0, pos);
    auto str_request_body = request_text.substr(pos + 4);

    // 处理请求头
    HTTP_REQUEST_HEADER header;
    int res = parse_request_header(str_request_header, header);
    if (res != 0)
    {
        return make_response("Bad Request", 400);
    }

    // 路由
    if (header.abs_path == "/" && header.http_type == SGET)
    {
        return make_response("index.html", HTTP_RESPONSE_HEADER(), TYPE_FILE);
    }
    else if (header.abs_path == "/login")
    {
        switch (header.http_type)
        {
        case SGET:
            return make_response("login.html", HTTP_RESPONSE_HEADER(), TYPE_FILE);
            break;
        case SPOST:
            return login(str_request_body);

            return make_response("index.html", HTTP_RESPONSE_HEADER(), TYPE_FILE);
            break;
        default:
            break;
        }
    }
    else if (header.abs_path == "/upload")
    {
        switch (header.http_type)
        {
        case SGET:
            return make_response("upload.html", HTTP_RESPONSE_HEADER(), TYPE_FILE);
            break;
        case SPOST:
            Log("处理upload post");
            return make_response("upload ok");
            break;
        default:
            break;
        }
    }
    else if (header.abs_path == "/text")
    {
        return make_response("text test");
    }
    else if (header.abs_path == "/baidu")
    {
        return make_response("302 test", "https://baidu.com/", 302);
    }
    else
    {
        return make_response(header.abs_path, HTTP_RESPONSE_HEADER(), TYPE_FILE);
    }

    return make_response("404", 404);
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
    Log(header_lines[0]);
    auto header_first_line = SplitString(header_lines[0], " ");
    if (header_first_line.size() < 3)
    {
        Log("invaild request header");
        return -1;
    }

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
    // 解析协议名称
    header.http_protocol = header_first_line[2];

    // 解析请求方法
    std::string str_request_type = header_first_line[0];
    if (str_request_type == "GET")
    {
        header.http_type = SGET;
    }
    else if (str_request_type == "POST")
    {
        header.http_type = SPOST;
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

// 构建响应
std::string ROUTES::make_response(HTTP_RESPONSE response)
{
    std::string str_header = "";
    std::string str_body = response.body;

    //构建响应头
    str_header = response.header.http_protocol + " " + get_str_status_code(response.header.response_code) + "\r\n";
    for (auto iter : response.header.header_params)
    {
        str_header += iter.first + ": " + iter.second + "\r\n";
    }

    str_header += "Content-Length: " + std::to_string(str_body.length()) + "\r\n";

    str_header += "\r\n";

    Log(get_str_status_code(response.header.response_code) + "\r\n");

    return str_header + str_body;
}

std::string ROUTES::make_response(std::string message, int status_code)
{
    return make_response(HTTP_RESPONSE(message, HTTP_RESPONSE_HEADER(status_code)));
}

std::string ROUTES::make_response(std::string message, std::string location, int status_code)
{
    return make_response(HTTP_RESPONSE(message, HTTP_RESPONSE_HEADER(status_code, "Location", location)));
}

std::string ROUTES::make_response(std::string str, HTTP_RESPONSE_HEADER header, TEXT_TYPE type)
{
    if (type == TYPE_TEXT)
    {
        return make_response(HTTP_RESPONSE(str, header));
    }

    auto instance = Config::get_instance();
    auto path = instance->DIR_PATH + str;

    Log("filepath: " + path);

    if (IsFileExists(path))
    {
        if (IsFileRead(path))
        {
            int pos = int(path.find_last_of('.'));
            if (pos == -1)
            {
                return make_response(ReadFile(path));
            }
            else
            {
                std::string file_type = path.substr(pos + 1);
                HTTP_RESPONSE_HEADER tmp_header;
                tmp_header.header_params["Content-Type"] = get_file_mime(file_type) + ";charset=utf-8";
                return make_response(ReadFile(path), tmp_header);
            }
        }
        else
        {
            return make_response("403 Forbidden", 403);
        }
    }
    else
    {
        return make_response("404 Not Found", 404);
    }
}

std::string ROUTES::get_file_mime(std::string file_type)
{
    auto instance = Config::get_instance();
    if (instance->content_type.find(file_type) != instance->content_type.end())
    {
        return instance->content_type[file_type];
    }
    return "null";
}

std::string ROUTES::get_str_status_code(int status)
{
    std::string status_line = "";
    if (status >= HTTP_OK && status < HTTP_LAST_2XX)
    {
        status_line = http_status_lines[status - HTTP_OK];
    }
    else if (status > HTTP_LAST_2XX && status < HTTP_LAST_3XX)
    {
        status_line = http_status_lines[status + HTTP_OFF_3XX - 301];
    }
    else if (status > HTTP_LAST_3XX && status < HTTP_LAST_4XX)
    {
        status_line = http_status_lines[status + HTTP_OFF_4XX - 400];
    }
    else if (status > HTTP_LAST_4XX && status < HTTP_LAST_5XX)
    {
        status_line = http_status_lines[status + HTTP_OFF_5XX - 500];
    }
    return status_line;
}

void ROUTES::ProcessRequest(HttpConn *conn)
{
    if (conn->request.abs_path == "/")
    {
        index(conn);
    }
    else if (conn->request.abs_path == "/upload")
    {
        upload(conn);
    }
    else
    {
        ProcessFile(conn);
    }
}

#pragma region 路由处理函数

void ROUTES::ProcessFile(HttpConn *conn)
{
    conn->response.file_path = conn->request.abs_path;
    string file_type = conn->response.file_path.substr(conn->response.file_path.find_last_of(".") + 1);
    conn->response.Content_Type = get_file_mime(file_type);
}

void ROUTES::index(HttpConn *conn)
{
    conn->request.abs_path = "/index.html";
    ProcessFile(conn);
}

void ROUTES::upload(HttpConn *conn)
{
}

#pragma endregion
