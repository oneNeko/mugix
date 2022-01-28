#ifndef HTTP_CONTROLER_H
#define HTTP_CONTROLER_H

#include "http_define.h"

namespace mugix::http
{
    //哈希表映射
    template <typename T1, typename T2>
    int ParseHeaderMap(T1 &key, T2 &hash_table)
    {
        if (hash_table.count(key) != 0)
        {
            return hash_table.at(key);
        }
        return 0;
    }

    //http控制类
    class HttpControler
    {
    private:
        HttpHeader header_;
    public:
        int ParseHeader(string header);
    };
}

#endif