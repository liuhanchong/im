/*
 * dbconnpool.h
 *
 *  Created on: 2015年9月8日
 *      Author: liuhanchong
 */

#ifndef DB_DBCONNPOOL_H_
#define DB_DBCONNPOOL_H_

#include "dbcore.h"
#include "../basic/queue.h"
#include "../config/configmanage.h"
#include "../thread/threadpool.h"
#include "../log/logmanage.h"

typedef struct DBConnNode
{
	MYSQL *pMySql;
	time_t tmAccessTime;
	int nConnection;//是否正在使用
} DBConnNode;

typedef struct DBConnPool
{
	List dbConnList;
	Thread *pFreeOvertimeConn;

	int nMaxConnNumber;
	int nCoreConnNumber;

	int nAccOverTime;/*连接未使用时间超时*/
	int nAccConnLoopSpace;/*连接未使用的判断间隔*/

	int nAddConnNumber;/*每次增加的链接数*/
} DBConnPool;

/*接口*/
int CreateDBConnPool(DBConnPool *pConnPool);
int ReleaseDBConnPool(DBConnPool *pConnPool);
DBConnNode *GetFreeDBConn(DBConnPool *pConnPool);/*获取一个空闲连接*/
int ReleaseAccessDBConn(DBConnNode *pDBConnNode);/*释放一个访问连接*/
void ReleaseDBConnNode(DBConnNode *pNode);/*释放连接池节点*/

/*私有*/
void *FreeDBConnAccess(void *pData);/*未访问超时连接*/
int CreateMulDBConn(DBConnPool *pConnPool, int nNumber);
int InsertDBConn(DBConnPool *pConnPool, char *pHost, char *pUser, char *pPasswd,
		char *pDB, char *pUnixSocket, unsigned long lClientFlag, unsigned int nPort);

#endif /* DB_DBCONNPOOL_H_ */
