#include "util.h"
#include "tpool.h"
#include <stdlib.h>

static void *defaulttfun(void *data)
{
	return NULL;
}

static int insertt(tpool *tpool)
{
	tnode *newtnode = (tnode *)malloc(sizeof(tnode));
	if (!newtnode)
	{
		return FAILED;
	}

	newtnode->thread = createthread(defaulttfun, newtnode, -1);
	if (!newtnode->thread)
	{
		free(newtnode);
		return FAILED;
	}

	newtnode->accesstime = time(NULL);
	newtnode->exetime = newtnode->accesstime;

	if (push(&tpool->tlist, (void *)newtnode, 0) == FAILED)
	{
		delthread(tpool, newtnode);
		return FAILED;
	}

	return SUCCESS;
}

static int getfreetnum(tpool *tpool)
{
	int count = 0;
	tnode *tnode = NULL;
	looplist_for(tpool->tlist)
	{
		tnode = (struct tnode *)headquenode->data;
		if (tnode && (isresume(tnode->thread) == 0))
		{
			count++;
		}
	}

	return count;
}

static tnode *getfreet(tpool *tpool)
{
	tnode *tnode = NULL;
	looplist_for(tpool->tlist)
	{
		tnode = (struct tnode *)headquenode->data;
		if (tnode && (isresume(tnode->thread) == 0))
		{
			break;
		}
		tnode = NULL;
	}

	return tnode;
}

tpool *createtpool(int maxtnum, int coretnum)
{
	tpool *newtpool = malloc(sizeof(tpool));
	if (!newtpool)
	{
		return NULL;
	}

	/*默认核心5个 最大10个*/
	maxtnum = (coretnum > maxtnum) ? coretnum : maxtnum;
	newtpool->coretnum = (coretnum > 0) ? coretnum : 5;
	newtpool->maxtnum = (maxtnum > 0) ? maxtnum : 10; 

	if (createqueue(&newtpool->tlist, maxtnum, 0, NULL) == FAILED ||
		addthread(newtpool, coretnum) == FAILED)
	{
		free(newtpool);
		return NULL;
	}

	return newtpool;
}

int destroytpool(tpool *tpool)
{
	if (!tpool)
	{
		return FAILED;
	}

	looplist_for(tpool->tlist)
	{
		struct tnode *tnode = (struct tnode *)headquenode->data;
		if (destroythread(tnode->thread) == FAILED)
		{
			debuginfo("destroytpool->destroythread failed");
		}
		free(tnode);
	}

	int ret = destroyqueue(&tpool->tlist);

	free(tpool);

	return ret;
}

int addttask(tpool *tpool, void *(*fun)(void *), void *data)
{
	if (!tpool)
	{
		return FAILED;
	}

	int ret = FAILED;
	lock(tpool->tlist.thmutex);
	tnode *tnode = getfreet(tpool);
	if (tnode)
	{
		setthreadexecute(tnode->thread, fun, data);

		if (enablethread(tnode->thread, 1) == SUCCESS)
		{
			tnode->accesstime = time(NULL);
			tnode->exetime = tnode->accesstime;

			ret = SUCCESS;
		}
	}
	unlock(tpool->tlist.thmutex);

	return ret;
}

int delthread(tpool *tpool, tnode *tnode)
{
	int ret = FAILED;
	lock(tpool->tlist.thmutex);
	looplist_for(tpool->tlist)
	{
		struct tnode *ftnode = (struct tnode *)headquenode->data;
		if (ftnode == tnode)
		{
			ret = destroythread(tnode->thread);
			free(tnode);
			dele(&tpool->tlist, headquenode);
			break;
		}
	}
	unlock(tpool->tlist.thmutex);
	return ret;
}

int addthread(tpool *tpool, int addtnum)
{
	if (tpool->maxtnum - addtnum < getcurqueuelen(&tpool->tlist))
	{
		return FAILED;
	}

	int ret = SUCCESS;
	lock(tpool->tlist.thmutex);
	for (int i = 1; i <= addtnum; i++)
	{
		if (insertt(tpool) == FAILED)
		{
			ret = FAILED;
			debuginfo("insert new thread failed seq=%d, total=%d", i, addtnum);
			break;
		}
	}
	unlock(tpool->tlist.thmutex);

	return ret;
}

