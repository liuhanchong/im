#include "dbpool.h"
extern "C"
{
	#include "../util.h"
}
#include <stdlib.h>
#include <string.h>

static char *copystr(char *str)
{
	int size = strlen(str);
	if (size < 1)
	{
		return NULL;
	}

	char *nstr = (char *)malloc(size + 1);
	memcpy(nstr, str, size);
	nstr[size] = '\0';

	return nstr;
}

static struct dbconn *createdbconn(struct dbconn *conn)
{
	struct dbconn *dbconn = (struct dbconn *)malloc(sizeof(struct dbconn));
	if (!dbconn)
	{
		return NULL;
	}

	dbconn->host = copystr(conn->host);
	dbconn->user = copystr(conn->user);
	dbconn->pass = copystr(conn->pass);
	dbconn->dbname = copystr(conn->dbname);
	dbconn->unixsock = copystr(conn->unixsock);
	dbconn->cliflag = conn->cliflag;
	dbconn->port = conn->port;

	return dbconn;
}

static void destroydbconn(struct dbconn *conn)
{
	free(conn->host);
	free(conn->user);
	free(conn->pass);
	free(conn->dbname);
	free(conn->unixsock);
	free(conn);
}

static int insertdb(struct dbpool *dbpool, struct dbconn *conn)
{
	struct dbnode *dbnode = (struct dbnode *)malloc(sizeof(struct dbnode));
	if (!dbnode)
	{
		return FAILED;
	}

	dbnode->conn = createdbconn(conn);
	if (!dbnode->conn)
	{
		free(dbnode);
		return FAILED;
	}

	switch (dbpool->dbtype)
	{
		case 1: 
			dbnode->db = new mydb();
			break;

		case 2:
			break;

		case 3:
			break;

		default:
			dbnode->db = new mydb();
			break;
	}

	if (!dbnode->db)
	{
		destroydbconn(dbnode->conn);
		free(dbnode);
		return FAILED;
	}

	if (dbnode->db->opendb(dbnode->conn) == FAILED)
	{
		destroydbconn(dbnode->conn);
		delete dbnode->db;
		free(dbnode);
		return FAILED;
	}

	dbnode->use = 0;

	if (push(&dbpool->dblist, dbnode, 0) == FAILED)
	{
		destroydbconn(dbnode->conn);
		dbnode->db->closedb();
		delete dbnode->db;
		free(dbnode);
		return FAILED;
	}

	return SUCCESS;
}

dbpool *createdbpool(int dbtype, int maxdbnum, int coredbnum, struct dbconn *conn)
{
	dbpool *newdbpool = (struct dbpool *)malloc(sizeof(dbpool));
	if (!newdbpool)
	{
		return NULL;
	}

	/*默认核心5个 最大10个*/
	maxdbnum = (coredbnum > maxdbnum) ? coredbnum : maxdbnum;
	newdbpool->coredbnum = (coredbnum > 0) ? coredbnum : 5;
	newdbpool->maxdbnum = (maxdbnum > 0) ? maxdbnum : 10; 
	newdbpool->dbtype = dbtype;

	if (createqueue(&newdbpool->dblist, maxdbnum, 0, NULL) == FAILED ||
		adddb(newdbpool, coredbnum, conn) == FAILED)
	{
		free(newdbpool);
		return NULL;
	}

	return newdbpool;
}

int destroydbpool(dbpool *dbpool)
{
	if (!dbpool)
	{
		return FAILED;
	}

	looplist_for(dbpool->dblist)
	{
		struct dbnode *dbnode = (struct dbnode *)headquenode->data;
		destroydbconn(dbnode->conn);
		if (dbnode->db->closedb() == FAILED)
		{
			debuginfo("destroydbpool->closedb failed");
		}
		delete dbnode->db;
		free(dbnode);
	}

	int ret = destroyqueue(&dbpool->dblist);

	free(dbpool);

	return ret;
}

int adddb(dbpool *dbpool, int adddbnum, struct dbconn *conn)
{
	for (int i = 1; i <= adddbnum; i++)
	{
		if (insertdb(dbpool, conn) == FAILED)
		{
			debuginfo("adddb failed, seq=%d", i);
		}
	}
	return SUCCESS;
}

int deldb(dbpool *dbpool, dbnode *dbnode)
{
	int ret = FAILED;
	lock(dbpool->dblist.thmutex);
	looplist_for(dbpool->dblist)
	{
		struct dbnode *fdbnode = (struct dbnode *)headquenode->data;
		if (fdbnode == dbnode)
		{
			ret = dbnode->db->closedb();
			free(dbnode);
			dele(&dbpool->dblist, headquenode);
			break;
		}
	}
	unlock(dbpool->dblist.thmutex);
	return ret;
}
