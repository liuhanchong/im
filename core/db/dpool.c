/*
 * dbconnpool.c
 *
 *  Created on: 2015年9月8日
 *      Author: liuhanchong
 */

#include "dbconnpool.h"

int CreateDBConnPool(DBConnPool *pConnPool)
{
	if (!pConnPool)
	{
		ErrorInfor("CreateDBConnPool", ERROR_ARGNULL);
		return 0;
	}

	pConnPool->nMaxConnNumber = serverConfig.nMaxConnNumber;
	pConnPool->nCoreConnNumber = serverConfig.nCoreConnNumber;

	pConnPool->nAccOverTime = serverConfig.nAccConnOverTime;
	pConnPool->nAccConnLoopSpace = serverConfig.nAccConnLoopSpace;

	pConnPool->nAddConnNumber = serverConfig.nAddConnNumber;

	if (InitQueue(&pConnPool->dbConnList, pConnPool->nMaxConnNumber, 0) == 0)
	{
		ErrorInfor("CreateDBConnPool", ERROR_INITQUEUE);
		return 0;
	}

	pConnPool->pFreeOvertimeConn = CreateLoopThread(FreeDBConnAccess, pConnPool, pConnPool->nAccConnLoopSpace);
	if (!pConnPool->pFreeOvertimeConn)
	{
		ReleaseQueue(&pConnPool->dbConnList);
		ErrorInfor("CreateDBConnPool", ERROR_CRETHREAD);
		return 0;
	}

	if (!CreateMulDBConn(pConnPool, pConnPool->nCoreConnNumber))
	{
		ReleaseThread(pConnPool->pFreeOvertimeConn);
		ReleaseQueue(&pConnPool->dbConnList);
		ErrorInfor("CreateDBConnPool", ERROR_CREPOOL);
		return 0;
	}

	return 1;
}

int ReleaseDBConnPool(DBConnPool *pConnPool)
{
	if (!pConnPool)
	{
		ErrorInfor("ReleaseDBConnPool", ERROR_ARGNULL);
		return 0;
	}

	if (pConnPool->pFreeOvertimeConn)
	{
		ReleaseThread(pConnPool->pFreeOvertimeConn);
	}

	/*遍历队列列表*/
	BeginTraveData(&pConnPool->dbConnList);
		ReleaseDBConnNode((DBConnNode *)pData);
	EndTraveData();

	ReleaseQueue(&pConnPool->dbConnList);

	return 1;
}

DBConnNode *GetFreeDBConn(DBConnPool *pConnPool)
{
	if (!pConnPool)
	{
		ErrorInfor("GetFreeDBConn", ERROR_ARGNULL);
		return 0;
	}

	/*遍历队列列表*/
	DBConnNode *pDBConnNode = NULL;
	LockQueue(&pConnPool->dbConnList);

	BeginTraveData(&pConnPool->dbConnList);
		pDBConnNode = (DBConnNode *)pData;
		if (pDBConnNode->nConnection == 0)
		{
			pDBConnNode->nConnection = 1;//状态为已连接
			pDBConnNode->tmAccessTime = time(NULL);
			break;
		}
		pDBConnNode = NULL;
	EndTraveData();

	/*当没有空闲连接，创建新的连接*/
	if (!pDBConnNode)
	{
		if (CreateMulDBConn(pConnPool, pConnPool->nAddConnNumber) == 0)
		{
			ErrorInfor("GetFreeDBConn", ERROR_CREPOOL);
		}
	}

	UnlockQueue(&pConnPool->dbConnList);

	return pDBConnNode;
}

int ReleaseAccessDBConn(DBConnNode *pDBConnNode)
{
	if (!pDBConnNode)
	{
		return 0;
	}

	pDBConnNode->nConnection = 0;//状态为已连接
	pDBConnNode->tmAccessTime = time(NULL);
	return 1;
}

void ReleaseDBConnNode(DBConnNode *pNode)
{
	if (pNode)
	{
		if (pNode->pMySql)
		{
			CloseDB(pNode->pMySql);
		}

		free(pNode);
		pNode = NULL;
	}
}

void *FreeDBConnAccess(void *pData)
{
	DBConnPool *pConnPool = (DBConnPool *)pData;
	if (!pConnPool)
	{
		ErrorInfor("FreeDBConnAccess", ERROR_TRANTYPE);
		return NULL;
	}

	/*遍历队列列表*/
	LockQueue(&pConnPool->dbConnList);

	//当前线程超过核心线程数删除
	if (GetCurQueueLen(&pConnPool->dbConnList) > pConnPool->nCoreConnNumber)
	{
		BeginTraveData(&pConnPool->dbConnList);
			time_t tmCurTime = time(NULL);
			DBConnNode *pDBConnNode = (DBConnNode *)pData;
			if (pDBConnNode->nConnection == 0)
			{
				if (tmCurTime - pDBConnNode->tmAccessTime >= pConnPool->nAccOverTime)
				{
					ReleaseDBConnNode(pDBConnNode);
					DeleteForNode(&pConnPool->dbConnList, pQueueNode);
				}
			}
		EndTraveData();
	}

	UnlockQueue(&pConnPool->dbConnList);

	return NULL;
}

int CreateMulDBConn(DBConnPool *pConnPool, int nNumber)
{
	if (!pConnPool)
	{
		ErrorInfor("CreateMulDBConn", ERROR_ARGNULL);
		return 0;
	}

	while ((nNumber--) > 0)
	{
		if (InsertDBConn(pConnPool, serverConfig.pHost, serverConfig.pUser, serverConfig.pPasswd,
				serverConfig.pDB, serverConfig.pUnixSocket, serverConfig.lClientFlag, serverConfig.nDBPort) == 0)
		{
			ErrorInfor("CreateMulDBConn", ERROR_CRETHREAD);
		}
	}

	return 1;
}

int InsertDBConn(DBConnPool *pConnPool, char *pHost, char *pUser, char *pPasswd, char *pDB,
		char *pUnixSocket, unsigned long lClientFlag, unsigned int nPort)
{
	if (!pConnPool)
	{
		ErrorInfor("InsertDBConn", ERROR_ARGNULL);
		return 0;
	}

	if (Full(&pConnPool->dbConnList))
	{
		ErrorInfor("InsertDBConn", ERROR_OUTQUEUE);
		return 0;
	}

	DBConnNode *pDBConnNode = (DBConnNode *)malloc(sizeof(DBConnNode));
	if (!pDBConnNode)
	{
		SystemErrorInfor("InsertDBConn", errno);
		return 0;
	}

	MYSQL *pMySql = OpenDB(pHost, pUser, pPasswd, pDB, pUnixSocket, lClientFlag, nPort);
	if (!pMySql)
	{
		free(pDBConnNode);
		pDBConnNode = NULL;

		ErrorInfor("InsertDBConn", ERROR_DBOPEN);
		return 0;
	}

	pDBConnNode->pMySql = pMySql;
	pDBConnNode->tmAccessTime = time(NULL);
	pDBConnNode->nConnection = 0;

	if (!Insert(&pConnPool->dbConnList, (void *)pDBConnNode, 0))
	{
		ReleaseDBConnNode(pDBConnNode);

		ErrorInfor("InsertDBConn", ERROR_INSEQUEUE);
		return 0;
	}

	return 1;
}
