
#ifndef __H_DATABASE__
#define __H_DATABASE__
#include <mysql/mysql.h>

#include "client.h"

void database_connect();

bool database_new_user(int8_t* username, int8_t* password);
bool database_user_login(client_t* client, int8_t* username, int8_t* password);
#endif