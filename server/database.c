
#include "database.h"

#include <mysql/mysql.h>
#include <stdio.h>

#include "client.h"

MYSQL* database_connect()
{
  char* server = "Ciro.local";
  char* user = "ciro";
  char* password = "ciro";
  char* database = "lso";
  
  MYSQL* conn = mysql_init(NULL);
  
  /* Connect to database */
  if (!mysql_real_connect(conn, server, user, password, database, 3306, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
  }

  printf("MySQL Connection estabilished\n");

  return conn;
}

void database_close(MYSQL* c)
{
  mysql_close(c);
}


bool database_new_user(char* username, char* password)
{
  char query[256];
  sprintf(query, "INSERT INTO users(username, password) VALUES('%s', '%s')", username, password);

  MYSQL* conn = database_connect();

  int result = mysql_query(conn, query);

  if(result)
  {
    fprintf(stderr, "%s\n", mysql_error(conn));  
  }

  database_close(conn);

  return result == 0;
}

bool database_user_login(char* username, char* password)
{
  char query[256];
  
  sprintf(query, "SELECT id FROM users where username='%s' AND password='%s' LIMIT 1", username, password);

  bool result = false;

  MYSQL* conn = database_connect();
  int q = mysql_query(conn, query);
  if(!q)
  {
    MYSQL_RES* res = mysql_use_result(conn);
    int numRows = mysql_num_rows(res);

    if(numRows > 0) 
    {
      result = true;
    }
  }

  database_close(conn);
  return true;
}