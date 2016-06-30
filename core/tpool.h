#ifndef TPOOL_H
#define TPOOL_H

#include "thread.h"
#include "queue.h"

typedef struct tnode
{
	thread *thread;
	time_t accesstime;
	time_t exetime;
} tnode;

typedef struct tpool
{
	list tlist;

	int maxtnum;/*最大线程数*/
	int coretnum;/*核心的线程数*/	
} tpool;

tpool *createtpool(int maxtnum, int coretnum);
int destroytpool(tpool *tpool);
int addttask(tpool *tpool, void *(*fun)(void *), void *data);/*为线程池添加任务*/
int addthread(tpool *tpool, int addtnum);/*添加线程*/
int delthread(tpool *tpool, tnode *tnode);/*删除线程*/

#endif /* TPOOL_H */
