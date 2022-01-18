#include "db_base.h"
class DB_Mysql : DB_Base
{
private:
    MYSQL mysql_;

public:
    DB_Mysql();
    ~DB_Mysql();

    int init();
    int query(char *data, const char *sql, ...) override;
};