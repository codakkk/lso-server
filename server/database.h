
#ifndef __H_DATABASE__
#define __H_DATABASE__
#include <mysql/mysql.h>

MYSQL* database_connect();
void database_close(MYSQL* c);


bool database_new_user(char* username, char* password);
bool database_user_login(char* username, char* password);
#endif