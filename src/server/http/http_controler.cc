#include "http_controler.h"

namespace mugix::http
{
    // 使用状态机解析http请求头
    int HttpControler::ParseHeader(string header)
    {
        int state = HEADER_START;
        int lpos = 0, rpos = 0;

        while (state != HEADER_END)
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
                header_.method_ = ParseHeaderMap(tmp_method, http_methods_);

                state = HEADER_PATH;
                lpos = rpos + 1;
                break;
            }
            case HEADER_PATH:
            {
                rpos = header.find(" ", lpos);
                header_.path_ = header.substr(lpos, rpos - lpos);

                state = HEADER_VERSION;
                lpos = rpos + 1;
                break;
            }
            case HEADER_VERSION:
            {
                rpos = header.find("\r\n", lpos);
                string tmp_version = header.substr(lpos, rpos - lpos);
                header_.version_ = ParseHeaderMap(tmp_version, http_versions_);

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
}