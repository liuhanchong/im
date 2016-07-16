#ifndef MYDB_H
#define MYDB_H

#include <mysql.h>
#include <my_global.h>
#include <my_sys.h>
#include <memory.h>
#include "../log/logmanage.h"
#include "../config/configmanage.h"

#define BeginTraveRecordResult(pMySql, pSql) \
		if (ExecuteSelect(pMySql, pSql) == 1) \
		{ \
			MYSQL_RES *pResult = GetRecordResult(pMySql); \
			if (GetRecordCount(pResult) > 0) \
			{ \
				MYSQL_ROW rowResult; \
				OffsetRecordResult(pResult, 0); \
				while ((rowResult = GetRowResult(pResult))) \
				{

#define EndTraveRecordResult() \
				} \
			} \
			ReleaseRecordResult(pResult); \
		}

//接口
MYSQL *OpenDB(char *pHost, char *pUser, char *pPasswd, char *pDB, char *pUnixSocket, unsigned long lClientFlag, unsigned int nPort);
int ExecuteSelect(MYSQL *pMySql, char *sSql);
int ExecuteModify(MYSQL *pMySql, char *sSql);
int ExecuteModifyEx(MYSQL *pMySql, char **sSqlArray, int nSize);
MYSQL_RES *GetRecordResult(MYSQL *pMySql);
void ReleaseRecordResult(MYSQL_RES *pResult);
unsigned long GetRecordCount(MYSQL_RES *pResult);
unsigned long GetAffectRow(MYSQL *pMySql);
char *GetStringValue(MYSQL_RES *pResult, char *pField);
int GetIntValue(MYSQL_RES *pResult, char *pField);
float GetFloatValue(MYSQL_RES *pResult, char *pField);
double GetDoubleValue(MYSQL_RES *pResult, char *pField);
int IsEOF(MYSQL_RES *pResult);
int IsActive(MYSQL *pMySql);
int ResetConnection(MYSQL *pMySql);
int OffsetRecordResult(MYSQL_RES *pResult, int nOffset);
MYSQL_ROW GetRowResult(MYSQL_RES *pResult);
const char *GetExecuteSqlResultInfor(MYSQL *pMySql);
int CloseDB(MYSQL *pMySql);

#endif /* DB_DBCORE_H_ */
