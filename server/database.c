
#include "database.h"

#include <mysql/mysql.h>
#include <stdio.h>

#include "client.h"

MYSQL* conn;

void database_connect()
{
  conn = mysql_init(NULL);

	if(conn == NULL)
	{
		printf("[Database] Unable to init MYSQL connection\n");
	}

  /* Connect to database */
  if (!mysql_real_connect(conn, "3.68.193.240", "postgres", "cirociro", "lso", 5432, NULL, 0)) {
  //if (!mysql_real_connect(conn, "Ciro.local", "root", "ciro", "lso", 5151, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
  }

  printf("[Database]: Connection estabilished\n");
}

bool database_new_user(int8_t* username, int8_t* password)
{
  char query[256];
  sprintf(query, "INSERT INTO users(username, password) VALUES('%s', '%s')", username, password);

  database_connect();

  int result = mysql_query(conn, query);

  if(result)
  {
    fprintf(stderr, "%s\n", mysql_error(conn));  
  }

  return result == 0;
}

bool database_user_login(client_t* client, int8_t* username, int8_t* password)
{
  char query[256];
  
  sprintf(query, "SELECT id, username FROM users where username='%s' AND password='%s' LIMIT 1", username, password);

	bool result = false;

  database_connect();
  int q = mysql_query(conn, query);
  if(q)
  {
    fprintf(stderr, "%s\n", mysql_error(conn));
    result = false;
  }
  else
  {
    MYSQL_RES* res = mysql_store_result(conn);
    
    uint64_t numRows = mysql_num_rows(res);

    if(numRows > 0) 
    {
      MYSQL_ROW row = mysql_fetch_row(res);

			result = true;
			client->user.id = atoi(row[0]);
			client->user.name = row[1];
    }
  }

  return result;
}