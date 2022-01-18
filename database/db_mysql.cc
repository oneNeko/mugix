#include <mysql/mysql.h>

#include "db_mysql.h"

DB_Mysql::DB_Mysql()
{
    mysql_init(&mysql_);
    if (!mysql_real_connect(&mysql_, host_, account_, password_, database_, port_, 0, 0))
    {
        mysql_error(&mysql_);
    }
    else
    {
        //cout << "mysql connected success" << endl;
    }
}

DB_Mysql::~DB_Mysql()
{
    mysql_close(&mysql_);
}

int DB_Mysql::query(char *data, const char *sql, ...)
{

    MYSQL_RES *res; //查询结果
    MYSQL_ROW row;  //一条数据记录，二维数组

    //查询
    //const char *sql0 = "select * from users";
    //int ret = mysql_query(&mysql, sql0);
    //std::cout<<mysql_error(&mysql);
    const char *sql1 = "select * from users";
    int ret = mysql_query(&mysql_, sql1);

    //获取结果
    res = mysql_store_result(&mysql_);
    while (row = mysql_fetch_row(res))
    {
        auto r0 = row[0];
        auto r1 = row[1];
    }

    mysql_free_result(res);

    return 0;
}