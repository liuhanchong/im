#ifndef MYDB_H
#define MYDB_H

#include <mysql.h>
#include <my_global.h>
#include <my_sys.h>

#define beginquery(mysql, sql) \
	if (querysql(mysql, sql) == SUCCESS) \
	{ \
		MYSQL_RES *result = getrecordresult(mysql); \
													\
		if (result && (getrecordcount(result) > 0)) \
		{ \
			MYSQL_ROW rowresult; \
			offrecordresult(result, 0); \
			while ((rowResult = getrowresult(result))) \
			{ \

#define endquery() \
			} \
			  \
			releaserecordresult(result); \
		} \
	} 

MYSQL *opendb(char *host, char *user, char *pass, char *dbname, 
	char *unixsock, unsigned long cliflag, unsigned int port);/*打开数据库*/
int querysql(MYSQL *mysql, char *sql);/*查询数据*/
int modifysql(MYSQL *mysql, char *sql);/*修改数据*/
int modifysqlex(MYSQL *mysql, char **sqlarray, int size);/*拓展的修改数据*/
MYSQL_RES *getrecordresult(MYSQL *mysql);/*获取结果集*/
void releaserecordresult(MYSQL_RES *result);/*释放结果集*/
unsigned long getrecordcount(MYSQL_RES *result);/*获取结果数量*/
unsigned long getaffectrow(MYSQL *mysql);/*sql操作影响的记录数*/
char *getstring(MYSQL_RES *result, char *field);/*获取字符串值*/
int getint(MYSQL_RES *result, char *field);/*获取int值*/
float getfloat(MYSQL_RES *result, char *field);/*获取float值*/
double getdouble(MYSQL_RES *result, char *field);/*获取double值*/
int iseof(MYSQL_RES *result);/*结果集是否到了末尾*/
int isactive(MYSQL *mysql);/*数据库通讯是否正常*/
int resetconn(MYSQL *mysql);/*重新连接数据库*/
int offrecordresult(MYSQL_RES *result, int off);/*偏移结果集*/
MYSQL_ROW getrowresult(MYSQL_RES *result);/*获取结果集的一行数据*/
const char *getexecuteresult(MYSQL *mysql);/*获取执行一条sql的结果*/
char *geterror(MYSQL *mysql);/*获取错误信息*/
int closedb(MYSQL *mysql);/*关闭数据库*/

#endif /* MYDB_H */
