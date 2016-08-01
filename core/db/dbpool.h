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

#endif /* DBPOOL_H */
