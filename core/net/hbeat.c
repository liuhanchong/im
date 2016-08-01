#include "hbeat.h"
#include "reactor.h"
#include "../util.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

static void *handlefd(void *data)
{
	heartbeat *hebeat = (heartbeat *)data;

	int timdif = 0;
	int rtconnnum = 0;
	for (int i = 0; i < hebeat->connnum; i++)
	{
		if (hebeat->fd[i][2] == 1)
		{
			rtconnnum++;			
		}
		
		if (hebeat->fd[i][2] != 1 ||
			time(NULL) - hebeat->fdtime[i] < hebeat->connouttime)
		{
			continue;
		}

		hebeat->fd[i][1]++;
		hebeat->fdtime[i] = time(NULL);

//		debuginfo("maxoutnum=%d sockid=%d failednum=%d", hebeat->maxouttcount, hebeat->fd[i][0], hebeat->fd[i][1]);

		if (hebeat->fd[i][1] >= hebeat->maxouttcount)
		{
			if (freeevent_ex(hebeat->fd[i][0], hebeat->reactor) == SUCCESS)
			{
				debuginfo("%s->%s success clientsock=%d", "handlefd", "outtime", hebeat->fd[i][0]);
			}

			rtconnnum--;
		}
	}

	debuginfo("exe handlefd");
	errorinfo("max conn num is %d, real time conn num is %d", hebeat->connnum, rtconnnum);

	return NULL;
}

heartbeat *createheartbeat(int connnum, int connouttime)
{
	connnum = (connnum > 0) ? connnum : 1024;
	connouttime = (connouttime > 0) ? connouttime : 1;

	heartbeat *newhb = (heartbeat *)malloc(sizeof(heartbeat));
	if (!newhb)
	{
		return NULL;
	}

	newhb->connnum = connnum;
	newhb->connouttime = connouttime;
	newhb->maxouttcount = 3;/*给定最多三次超时*/

	/*创建二维数组*/
	int fdsize = connnum * INFOTYPENUM * sizeof(int);
	newhb->fd = (int (*)[INFOTYPENUM])malloc(fdsize);
	if (!newhb->fd)
	{
		free(newhb);
		return NULL;
	}
	memset(newhb->fd, 0, fdsize);

	newhb->fdtime = (time_t *)malloc(connnum * sizeof(time_t));
	if (!newhb->fdtime)
	{
		free(newhb->fd);
		free(newhb);
		return NULL;
	}

	newhb->hbthread = createthread(handlefd, newhb, connouttime);
	if (!newhb->hbthread)
	{
		free(newhb->fd);
		free(newhb);
		return NULL;
	}

	return newhb;
}

int destroyheartbeat(heartbeat *hebeat)
{
	if (!hebeat)
	{
		return FAILED;
	}

	destroythread(hebeat->hbthread);

	free(hebeat->fd);
	
	free(hebeat->fdtime);

	free(hebeat);

	return SUCCESS;
}

int addheartbeat(heartbeat *hebeat, int fd)
{
	for (int i = 0; i < hebeat->connnum; i++)
	{
		if (hebeat->fd[i][2] == 0)
		{
			hebeat->fd[i][0] = fd;
			hebeat->fd[i][1] = 0;
			hebeat->fd[i][2] = 1;
			hebeat->fdtime[i] = time(NULL);
			break;
		}
	}

	return SUCCESS;
}

int delheartbeat(heartbeat *hebeat, int fd)
{
	for (int i = 0; i < hebeat->connnum; i++)
	{
		if (hebeat->fd[i][0] == fd &&
			 hebeat->fd[i][2] == 1)
		{
			hebeat->fd[i][2] = 0;
			break;
		}
	}

	return SUCCESS;
}

int upheartbeat(heartbeat *hebeat, int fd)
{
	for (int i = 0; i < hebeat->connnum; i++)
	{
		if (hebeat->fd[i][0] == fd &&
			 hebeat->fd[i][2] == 1)
		{
			hebeat->fd[i][1] = 0;
			hebeat->fdtime[i] = time(NULL);
			break;
		}
	}

	return SUCCESS;
}