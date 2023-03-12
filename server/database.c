
#include "database.h"

#include <mysql/mysql.h>
#include <stdio.h>

#include "client.h"

MYSQL* database_connect()
{
  char* server = "Ciro.local";
  char* user = "root";
  char* password = "ciro";
  char* database = "lso";
  
  MYSQL* conn = mysql_init(NULL);
  
  /* Connect to database */
  if (!mysql_real_connect(conn, server, user, password, database, 5151, NULL, 0)) {
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

user_t* database_user_login(char* username, char* password)
{
  char query[256];
  
  sprintf(query, "SELECT id, username FROM users where username='%s' AND password='%s' LIMIT 1", username, password);

  user_t* result = NULL;

  MYSQL* conn = database_connect();
  int q = mysql_query(conn, query);
  if(q)
  {
    fprintf(stderr, "%s\n", mysql_error(conn));
    result = NULL;
  }
  else
  {
    MYSQL_RES* res = mysql_store_result(conn);
    
    int numRows = mysql_num_rows(res);

    if(numRows > 0) 
    {
      int numFields = mysql_num_fields(res);
      MYSQL_ROW row = mysql_fetch_row(res);

      result = malloc(sizeof(user_t));
      result->id = atoi(row[0]);
      result->name = row[1];
    }
  }

  database_close(conn);
  return result;
}