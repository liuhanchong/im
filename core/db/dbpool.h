#ifndef DBPOOL_H
#define DBPOOL_H

#include "db.h"
#include "mydb.h"
extern "C"
{
	#include "../queue.h"
}

typedef struct dbnode
{
	struct dbconn *conn;
	db *db;
	int use;/*使用标志 0-未使用 1-正在使用*/
} dbnode;

typedef struct dbpool
{
	list dblist;
	int maxdbnum;/*最大db数*/
	int coredbnum;/*核心的db数*/
	int dbtype;/*1-mysql 2-oracle 3-sqlserver 4-...*/
} dbpool;

dbpool *createdbpool(int dbtype, int maxdbnum, int coredbnum, struct dbconn *conn);
int destroydbpool(dbpool *dbpool);
int adddb(dbpool *dbpool, int adddbnum, struct dbconn *conn);/*添加db*/
int deldb(dbpool *dbpool, dbnode *dbnode);/*删除db*/
dbnode *getdb(dbpool *dbpool);/*获取一个未使用的连接*/
void reldb(dbpool *dbpool, dbnode *dbnode);/*释放不使用的db*/
int getdbpcurlen(dbpool *dbpool);/*获取池长度*/
int getdbpmaxlen(dbpool *dbpool);/*获取最大长度*/
int getdbpflen(dbpool *dbpool);/*获取池自由的个数*/

#endif /* DBPOOL_H */
