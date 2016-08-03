#include "../core/db/dbpool.h"
extern "C"
{
	#include "../core/util.h"
}
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	openlog();

	struct dbconn *conn = (struct dbconn *)malloc(sizeof(struct dbconn));
	if (!conn)
	{
		debuginfo("create dbconn failed");
		return 0;
	}

	conn->host = (char *)"127.0.0.1";
	conn->user = (char *)"root";
	conn->pass = (char *)"root";
	conn->dbname = (char *)"im";
	conn->unixsock = (char *)"";
	conn->cliflag = 0;
	conn->port = 3306;

	dbpool *dbpool = createdbpool(1, 10, 5, conn);
	if (!dbpool)
	{
		debuginfo("createdbpool failed");
		return 0;
	}

	debuginfo("no use db is %d, cur len=%d, max len=%d", getdbpflen(dbpool),
	 getdbpcurlen(dbpool), getdbpmaxlen(dbpool));

	dbnode *dbnode1 = getdb(dbpool);
	db *db1 = dbnode1->db;
	if (!db1)
	{
		debuginfo("create db failed");
		return 0;
	}

	debuginfo("no use db is %d, cur len=%d, max len=%d", getdbpflen(dbpool),
	 getdbpcurlen(dbpool), getdbpmaxlen(dbpool));

	char *sql = NULL;//(char *)"insert into message(id, messtype, message) value('1', 2, '2345')";
	if (db1->modifysql(sql) == FAILED)
	{
		debuginfo("exe sql failed");
	}

	struct dbnode *dbnode2 = getdb(dbpool);
	db *db2 = dbnode2->db;
	char *sql1[1] = {(char *)"insert into message(id, messtype, message) value('2', 2, '2345')"};
	if (db2->modifysqlex(sql1, 1) == FAILED)
	{
		debuginfo("exe sql failed");
	}

	dbnode *dbnode3 = getdb(dbpool);
	db *db3 = dbnode3->db;
	char *sql2 = (char *)"select * from message t where 1=1";
	if (db3->querysql(sql2) == SUCCESS)
	{
		if (db3->getrecordresult() == SUCCESS)
		{
			while (db3->iseof())
			{
				debuginfo("id=%s, messtype=%d, message=%s",
				 db3->getstring((char *)"id"), db3->getint((char *)"messtype"), db3->getstring((char *)"message"));
				
				db3->nextrow();
			}
			db3->releaserecordresult();
		}
	}

	debuginfo("no use db is %d, cur len=%d, max len=%d", getdbpflen(dbpool),
	 getdbpcurlen(dbpool), getdbpmaxlen(dbpool));

	mydb *mdb = (mydb *)db1;
	debuginfo("db state is act=%d,er=%s", mdb->isactive(), mdb->getexecuteresult());

	reldb(dbpool, dbnode1);

	debuginfo("no use db is %d, cur len=%d, max len=%d", getdbpflen(dbpool),
	 getdbpcurlen(dbpool), getdbpmaxlen(dbpool));

	mdb = (mydb *)db2;
	debuginfo("db state is act=%d,er=%s", mdb->isactive(), mdb->getexecuteresult());

	mdb = (mydb *)db3;
	debuginfo("db state is act=%d,er=%s", mdb->isactive(), mdb->getexecuteresult());

	reldb(dbpool, dbnode2);
	reldb(dbpool, dbnode3);

	debuginfo("no use db is %d, cur len=%d, max len=%d", getdbpflen(dbpool),
	 getdbpcurlen(dbpool), getdbpmaxlen(dbpool));

	for (int i = 0; i < 3; i++)
	{
		dbnode *dbn = getdb(dbpool);
		deldb(dbpool, dbn);
	}

	debuginfo("no use db is %d, cur len=%d, max len=%d", getdbpflen(dbpool),
	 getdbpcurlen(dbpool), getdbpmaxlen(dbpool));

	for (int i = 0; i < getdbpcurlen(dbpool) + 1; i++)
	{
		dbnode *dbn = getdb(dbpool);
		if (dbn == NULL)
		{
			debuginfo("no free db %d", i);
		}
		else
		{
			reldb(dbpool, dbn);
		}
	}

	debuginfo("no use db is %d, cur len=%d, max len=%d", getdbpflen(dbpool),
	 getdbpcurlen(dbpool), getdbpmaxlen(dbpool));

	for (int i = 0; i < getdbpcurlen(dbpool) + 1; i++)
	{
		dbnode *dbn = getdb(dbpool);
		if (dbn == NULL)
		{
			debuginfo("no free db %d", i);
		}
	}

	debuginfo("no use db is %d, cur len=%d, max len=%d", getdbpflen(dbpool),
	 getdbpcurlen(dbpool), getdbpmaxlen(dbpool));

	if (destroydbpool(dbpool) == FAILED)
	{
		debuginfo("destroy dbpool failed");
		return 0;
	}

	debuginfo("destroy dbpool success");

	closelog();

	return 0;
}