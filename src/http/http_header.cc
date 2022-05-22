#include "http_header.h"

namespace mugix
{
    HttpHeader::HttpHeader()
    {
    }

    HttpHeader::HttpHeader(const string &header)
    {
        ParseHeader(header);
    }

    // 使用状态机解析http请求头
    int HttpHeader::ParseHeader(const string &header)
    {
        int state = HEADER_START;
        int lpos = 0, rpos = 0;

        if (header.find("\r\n") == string::npos || header.find(" ") == string::npos)
        {
            return -1;
        }

        while (state != HEADER_END && lpos < header.length())
        {
            switch (state)
            {
            case HEADER_START:
            {
                state = HEADER_METHOD;
                break;
            }
            case HEADER_METHOD:
            {
                rpos = header.find(" ", lpos);
                string tmp_method = header.substr(lpos, rpos - lpos);
                method_ = ParseHeaderMap(tmp_method, http_methods_);
                if (method_ == HTTP_METHOD_NOT_SUPPORT)
                {
                    return -1;
                }

                state = HEADER_PATH;
                lpos = rpos + 1;
                break;
            }
            case HEADER_PATH:
            {
                rpos = header.find(" ", lpos);
                path_ = header.substr(lpos, rpos - lpos);

                state = HEADER_VERSION;
                lpos = rpos + 1;
                break;
            }
            case HEADER_VERSION:
            {
                rpos = header.find("\r\n", lpos);
                string tmp_version = header.substr(lpos, rpos - lpos);
                version_ = ParseHeaderMap(tmp_version, http_versions_);
                if (version_ == HTTP_VERSION_NOT_SUPPORT)
                {
                    return -1;
                }

                state = HEADER_OTHER_PARAMS;
                lpos = rpos + 2;
                break;
            }
            case HEADER_OTHER_PARAMS:
            {
                state = HEADER_END;
                break;
            }
            default:
            {
                state = HEADER_END;
                break;
            }
            }
        }
        return 0;
    }
}// namespace mugix