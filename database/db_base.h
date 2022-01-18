class DB_Base
{
protected:
    int type_;        //数据库类型
    char* account_;  //数据库账号
    char* password_; //数据库密码
    char* host_;     //数据库主机
    int port_;      //数据库端口
    char* database_; //数据库
protected:
    // 查询数据库
    // sql 查询语句
    // data 返回数据
    virtual int query(char* data, const char *sql, ...) = 0;

    //virtual int update(const string sql) = 0;
    //virtual int delete_data(const string sql) = 0;
};