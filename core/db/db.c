#include "mydb.h"

MYSQL *OpenDB(char *pHost, char *pUser, char *pPasswd, char *pDB, char *pUnixSocket, unsigned long lClientFlag, unsigned int nPort)
{
	MYSQL *pMySql = mysql_init(NULL);
	if (!pMySql)
	{
		SystemErrorInfor("OpenDB", errno);
		return NULL;
	}

	if (!mysql_real_connect(pMySql, pHost, pUser, pPasswd, pDB, nPort, pUnixSocket, lClientFlag))
	{
		ErrorInfor("OpenDB-1", (char *)mysql_error(pMySql));
		CloseDB(pMySql);
		return NULL;
	}

	if (mysql_autocommit(pMySql, 0) != 0)
	{
		ErrorInfor("OpenDB-2", (char *)mysql_error(pMySql));
		CloseDB(pMySql);
		return NULL;
	}

	return pMySql;
}

int ExecuteSelect(MYSQL *pMySql, char *sSql)
{
	if (!sSql || !pMySql)
	{
		ErrorInfor("ExecuteSelect", ERROR_ARGNULL);
		return 0;
	}

	if (mysql_query(pMySql, sSql) == 0)
	{
		return 1;
	}

	ErrorInfor("ExecuteSelect", (char *)mysql_error(pMySql));
	return 0;
}

int ExecuteModify(MYSQL *pMySql, char *sSql)
{
	if (!sSql || !pMySql)
	{
		ErrorInfor("ExecuteModify", ERROR_ARGNULL);
		return 0;
	}

	if (mysql_real_query(pMySql, sSql, strlen(sSql)) != 0)
	{
		ErrorInfor("ExecuteModify-1", (char *)mysql_error(pMySql));
		return 0;
	}

	if (mysql_commit(pMySql) != 0)
	{
		ErrorInfor("ExecuteModify-2", (char *)mysql_error(pMySql));
		return 0;
	}

	return 1;
}

int ExecuteModifyEx(MYSQL *pMySql, char **sSqlArray, int nSize)
{
	if (!sSqlArray || !pMySql || nSize <= 0)
	{
		ErrorInfor("ExecuteModifyEx", ERROR_ARGNULL);
		return 0;
	}

	for (int i = 0; i < nSize; i++)
	{
		if (mysql_query(pMySql, sSqlArray[i]) != 0)
		{
			ErrorInfor("ExecuteModifyEx-1", (char *)mysql_error(pMySql));

			if (mysql_rollback(pMySql) != 0)
			{
				ErrorInfor("ExecuteModifyEx-2", (char *)mysql_error(pMySql));
			}
			return 0;
		}
	}

	if (mysql_commit(pMySql) != 0)
	{
		ErrorInfor("ExecuteModifyEx-3", (char *)mysql_error(pMySql));
		return 0;
	}

	return 1;
}

unsigned long GetAffectRow(MYSQL *pMySql)
{
	if (!pMySql)
	{
		ErrorInfor("GetEffectRow", ERROR_ARGNULL);
		return 0;
	}

	return mysql_affected_rows(pMySql);
}

unsigned long GetRecordCount(MYSQL_RES *pResult)
{
	if (!pResult)
	{
		ErrorInfor("GetRecordCount", ERROR_ARGNULL);
		return 0;
	}

	return mysql_num_rows(pResult);
}

MYSQL_RES *GetRecordResult(MYSQL *pMySql)
{
	if (!pMySql)
	{
		ErrorInfor("GetRecordResult", ERROR_ARGNULL);
		return NULL;
	}

	MYSQL_RES *pResult = mysql_store_result(pMySql);
	if (!pResult)
	{
		ErrorInfor("GetRecordResult", (char *)mysql_error(pMySql));
	}
	return pResult;
}

void ReleaseRecordResult(MYSQL_RES *pResult)
{
	if (!pResult)
	{
		ErrorInfor("ReleaseRecordResult", ERROR_ARGNULL);
	}
	else
	{
		mysql_free_result(pResult);
	}
}

char *GetStringValue(MYSQL_RES *pResult, char *pField)
{
	if (!pResult || !pField)
	{
		ErrorInfor("GetStringValue", ERROR_ARGNULL);
		return NULL;
	}

	unsigned int nColumns = mysql_num_fields(pResult);
	MYSQL_FIELD *pFields = mysql_fetch_fields(pResult);
	if (!pFields)
	{
		ErrorInfor("GetStringValue", ERROR_FILEDDB);
		return NULL;
	}

	char *pValue = NULL;
	MYSQL_ROW rowResult = pResult->current_row;
	if (rowResult && pFields)
	{
		for (unsigned int i = 0; i < nColumns; i++)
		{
			if (strcmp(pFields[i].name, pField) == 0)
			{
				pValue = rowResult[i];
				break;
			}
		}
	}

	return pValue;
}

int GetIntValue(MYSQL_RES *pResult, char *pField)
{
	char *pValue = GetStringValue(pResult, pField);
	if (pValue)
	{
		return atoi(pValue);
	}

	return 0;
}

double GetDoubleValue(MYSQL_RES *pResult, char *pField)
{
	char *pValue = GetStringValue(pResult, pField);
	if (pValue)
	{
		return atof(pValue);
	}

	return 0;
}

float GetFloatValue(MYSQL_RES *pResult, char *pField)
{
	char *pValue = GetStringValue(pResult, pField);
	if (pValue)
	{
		return (float)atof(pValue);
	}

	return 0;
}

int IsEOF(MYSQL_RES *pResult)
{
	if (!pResult)
	{
		ErrorInfor("IsEOF", ERROR_ARGNULL);
		return 1;
	}

	return (mysql_eof(pResult) == 0) ? 0 : 1;
}

int IsActive(MYSQL *pMySql)
{
	if (!pMySql)
	{
		ErrorInfor("IsActive", ERROR_ARGNULL);
		return 0;
	}

	if (mysql_ping(pMySql) != 0)
	{
		ErrorInfor("IsActive", (char *)mysql_error(pMySql));
		return 0;
	}

	return 1;
}

int ResetConnection(MYSQL *pMySql)
{
	if (!pMySql)
	{
		ErrorInfor("ResetConnection", ERROR_ARGNULL);
		return 0;
	}

//	if (mysql_reset_connection(pMySql) != 0)
	{
		ErrorInfor("ResetConnection", (char *)mysql_error(pMySql));
		return 0;
	}

	return 1;
}

int OffsetRecordResult(MYSQL_RES *pResult, int nOffset)
{
	if (!pResult || nOffset < 0)
	{
		ErrorInfor("OffsetRecordResult", ERROR_ARGNULL);
		return 0;
	}

	mysql_data_seek(pResult, nOffset);

	return 1;
}

MYSQL_ROW GetRowResult(MYSQL_RES *pResult)
{
	if (!pResult)
	{
		ErrorInfor("GetRowResult", ERROR_ARGNULL);
		return NULL;
	}

	return mysql_fetch_row(pResult);
}

const char *GetExecuteSqlResultInfor(MYSQL *pMySql)
{
	return mysql_info(pMySql);
}

int CloseDB(MYSQL *pMySql)
{
	if (!pMySql)
	{
		ErrorInfor("CloseDB", ERROR_ARGNULL);
		return 0;
	}

	mysql_close(pMySql);
	return 1;
}

