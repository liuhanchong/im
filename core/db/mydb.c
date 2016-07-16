#include "mydb.h"
#include "util.h"

MYSQL *opendb(char *host, char *user, char *pass, char *dbname, 
	char *unixsock, unsigned long cliflag, unsigned int port)
{
	MYSQL *mysql = mysql_init(NULL);
	if (!mysql)
	{
		return NULL;
	}

	if (!mysql_real_connect(mysql, host, user, pass, dbname, port, unixsock, cliflag))
	{
		closedb(mysql);
		return NULL;
	}

	if (mysql_autocommit(mysql, 0) != 0)
	{
		closedb(mysql);
		return NULL;
	}

	return mysql;
}

int querysql(MYSQL *mysql, char *sql)
{
	if (!sql || !mysql)
	{
		debuginfo("querysql fun arg is null");
		return FAILED;
	}

	if (mysql_query(mysql, sql) != 0)
	{
		return FAILED;
	}

	return SUCCESS;
}

int modifysql(MYSQL *mysql, char *sql)
{
	if (!sql || !mysql)
	{
		debuginfo("modiftysql fun arg is null");
		return FAILED;
	}

	if (mysql_real_query(mysql, sql, strlen(sql)) != 0)
	{
		return FAILED;
	}

	if (mysql_commit(mysql) != 0)
	{
		return FAILED;
	}

	return SUCCESS;
}

int modifysqlex(MYSQL *mysql, char **sqlarray, int size)
{
	if (!sqlarray || !mysql || size <= 0)
	{
		debuginfo("modiftysqlex fun arg is null");
		return FAILED;
	}

	for (int i = 0; i < size; i++)
	{
		if (mysql_query(mysql, sqlarray[i]) != 0)
		{
			debuginfo("modifysqlex execute sql failed, sql=%s", sql);

			if (mysql_rollback(mysql) != 0)
			{
				debuginfo("modifysqlex rollback failed");
			}
			return FAILED;
		}
	}

	if (mysql_commit(mysql) != 0)
	{
		debuginfo("modifysqlex commit failed");
		return FAILED;
	}

	return SUCCESS;
}

MYSQL_RES *getrecordresult(MYSQL *mysql)
{
	return mysql_store_result(mysql);
}

void ReleaseRecordResult(MYSQL_RES *result)
{
	mysql_free_result(result);
}

unsigned long getrecordcount(MYSQL_RES *result)
{
	return mysql_num_rows(result);
}

unsigned long getaffectrow(MYSQL *mysql)
{
	return mysql_affected_rows(mysql);
}

char *getstring(MYSQL_RES *result, char *field)
{
	if (!result || !field)
	{
		return NULL;
	}

	unsigned int cols = mysql_num_fields(result);
	MYSQL_FIELD *fields = mysql_fetch_fields(result);
	MYSQL_ROW rowresult = result->current_row;
	for (unsigned int i = 0; i < cols; i++)
	{
		if (strcmp(fields[i].name, field) == 0)
		{
			return rowresult[i];
		}
	}

	return NULL;
}

int getint(MYSQL_RES *result, char *field)
{
	return atoi(getstring(result, field));
}

float getfloat(MYSQL_RES *result, char *field)
{
	return (float)atof(getstring(result, field));
}

double getdouble(MYSQL_RES *result, char *field)
{
	return atoi(getstring(result, field));
}

int iseof(MYSQL_RES *result)
{
	return 1;
}

int isactive(MYSQL *mysql)
{
	return (mysql_ping(mysql) == 0) ? 1 : 0;
}

int resetconn(MYSQL *mysql)
{
	//mysql_reset_connection(pMySql);
	return 1;
}

int offrecordresult(MYSQL_RES *result, int off)
{
	mysql_data_seek(result, off);
	return 1;
}

MYSQL_ROW getrowresult(MYSQL_RES *result)
{
	return mysql_fetch_row(result);
}

const char *getexecuteresult(MYSQL *mysql)
{
	return mysql_info(mysql);
}

char *geterror(MYSQL *mysql)
{
	return (char *)mysql_error(mysql);
}

int closedb(MYSQL *mysql)
{
	mysql_close(mysql);
	return 1;
}

