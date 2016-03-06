#include "reactor.h"
#include "common.h"
#include "socket.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <math.h>

static int timespeccompare(struct timespec *src, struct timespec *dest)
{
	if (src->tv_sec < dest->tv_sec)
	{
		return 1;
	}
	
	if (src->tv_sec > dest->tv_sec)
	{
		return -1;
	}
	
	if (src->tv_nsec < dest->tv_nsec)
	{
		return 1;
	}
	
	if (src->tv_nsec > dest->tv_nsec)
	{
		return -1;
	}

	return 0;
}

static int timercompare(queuenode *src, queuenode *dest)
{
	event *srcuevent = (struct event *)src->data;
	event *destuevent = (struct event *)dest->data;

	return timespeccompare(&srcuevent->timer, &destuevent->timer);
}

/*设置计时器事件*/
event *settimer(reactor *reactor, void *(*callback)(void *), void *arg, struct timespec *timer)
{
	struct event *uevent = malloc(sizeof(struct event));
	if (uevent == NULL)
	{
		return NULL;
	}

	uevent->fd = -1;
	uevent->reactor = reactor;
	uevent->callback = callback;
	uevent->arg = arg;
	memcpy(&uevent->timer, timer, sizeof(struct timespec));
	uevent->eventflag = 0;
	uevent->next = NULL;

	return uevent;
}

/*添加计时器事件*/
int addtimer(event *uevent, int eventflag)
{
	uevent->eventflag = EV_TIMEOUT;
	if (eventflag & EV_PERSIST)
	{
		uevent->eventflag |= EV_PERSIST;
	}

	//设置计时器时间
	time_t curtime = time(NULL);
	uevent->timer.tv_sec += curtime;

	//加入到用户事件列表
	return push(&uevent->reactor->utimersevelist, uevent, 0);
}

/*删除计时器事件*/
int deltimer(event *uevent)
{
	queuenode *headquenode = gethead(&uevent->reactor->utimersevelist);
	while (headquenode)
	{
		struct event *headuevent = (struct event *)headquenode->data;

		// if (uevent->callback == headuevent->callback 
		// 	&& (timespeccompare(&uevent->timer, &headuevent->timer) == 0))
		if (headuevent == uevent)
		{
			delete(&uevent->reactor->utimersevelist, headquenode);
			return SUCCESS;
		}

		if ((headquenode = headquenode->next) == gethead(&uevent->reactor->utimersevelist))
		{
			break;
		}
	}

	return FAILED;
}

/*获取最小超时时间*/
int getminouttimers(reactor *reactor, struct timespec *mintimer)
{
	//计算最小超时时间
	queuenode *headquenode = gethead(&reactor->utimersevelist);
	if (headquenode == NULL)
	{
		return FAILED;
	}

	struct timespec headtimer = ((struct event *)headquenode->data)->timer;
	time_t curtime = time(NULL);

	if (headtimer.tv_nsec > 0)
	{
		mintimer->tv_nsec = (long)(pow(10, 9) - headtimer.tv_nsec);
		mintimer->tv_sec = headtimer.tv_sec - curtime - 1;
	}
	else
	{
		mintimer->tv_sec = headtimer.tv_sec - curtime;
	}

	if (mintimer->tv_sec < 0)
	{
		mintimer->tv_sec = 0;	
		mintimer->tv_nsec = 0;
	}

	return SUCCESS;
}

/*遍历计时器事件*/
int looptimers(reactor *reactor)
{
	struct timespec curtime = {time(NULL), 0};

	queuenode *headquenode = gethead(&reactor->utimersevelist);
	while (headquenode)
	{
		struct event *headuevent = (struct event *)headquenode->data;
		if (timespeccompare(&curtime, &headuevent->timer) == -1)
		{
			push(&reactor->uactevelist, headuevent, 0);
		}
		else
		{
			break;
		}

		if ((headquenode = headquenode->next) == gethead(&reactor->utimersevelist))
		{
			break;
		}
	}

	return SUCCESS;
}

/*获取指定的socket事件在hash表位置*/
static struct event *gethashevent(int fd, reactor *reactor)
{
	int hashindex = fd % reactor->uevelistlen;
	return &(reactor->uevelist[hashindex]);
}

/*创建反应堆*/
reactor *createreactor()
{
	reactor *newreactor = (struct reactor *)malloc(sizeof(reactor));
	if (newreactor == NULL)
	{
		return NULL;
	}

	newreactor->reactorid = kqueue();
	if (newreactor->reactorid == -1)
	{
		return NULL;
	}

	/*使用进程最大打开文件数目*/
	int evenumber = getmaxfilenumber();
	if (evenumber <= 0)
	{
		evenumber = 256;
	}

	/*初始化hash表 链地址法处理冲突*/
	newreactor->uevelistlen = evenumber;
	int uevelistsize = sizeof(struct event) * evenumber;
	newreactor->uevelist = (struct event *)malloc(uevelistsize);
	if (newreactor->uevelist == NULL)
	{
		return NULL;
	}
	memset(newreactor->uevelist, 0, uevelistsize);

	newreactor->kevelistlen = evenumber;
	newreactor->kevelist = (struct kevent *)malloc(sizeof(struct kevent) * evenumber);
	if (newreactor->kevelist == NULL)
	{
		return NULL;
	}

	if (createqueue(&newreactor->uactevelist, 0, 0, NULL) == FAILED)
	{
		return NULL;
	}

	if (createqueue(&newreactor->utimersevelist, 0, 2, timercompare) == FAILED)
	{
		return NULL;
	}

	/*开启监听事件*/
	newreactor->listen = 1;
	
	return newreactor;
}

/*设置事件*/
event *setevent(reactor *reactor, int fd, void *(*callback)(void *), void *arg)
{
	struct event *uevent = malloc(sizeof(struct event));
	if (uevent == NULL)
	{
		return NULL;
	}

	uevent->fd = fd;
	uevent->callback = callback;
	uevent->arg = arg;
	uevent->reactor = reactor;
	uevent->readbufsize = 0;
	uevent->writebufsize = 0;
	uevent->next = NULL;
	uevent->eventflag = 0;

	return uevent;
}

/*添加事件*/
int addevent(event *uevent, int eventflag)
{
	uevent->eventflag = eventflag;

	//获取监听的事件
	if (eventflag & EV_READ)
	{
		eventflag = EVFILT_READ;
	}
	else if (eventflag & EV_WRITE)
	{
		eventflag = EVFILT_WRITE;
	}
	else if (eventflag & EV_SIGNAL)
	{
		eventflag = EVFILT_SIGNAL;
	}

	//设置添加操作
	int eventoper = (eventflag & EV_PERSIST) ? EV_ADD : EV_ADD | EV_ONESHOT; 
	
	//加入到系统内核监听
	struct kevent addevent;
	EV_SET(&addevent, uevent->fd, eventflag, eventoper, 0, 0, 0);
	if (kevent(uevent->reactor->reactorid, &addevent, 1, NULL, 0, NULL) == -1)
	{
		return FAILED;
	}

	//加入到用户事件列表
	struct event *hashuevent = gethashevent(uevent->fd, uevent->reactor);
	while (hashuevent->next)
	{
		hashuevent = hashuevent->next;
	}
	hashuevent->next = uevent;
	
	return SUCCESS;
}

/*删除事件*/
int delevent(event *uevent)
{
	struct event *prehashuevent = gethashevent(uevent->fd, uevent->reactor);
	struct event *hashuevent = prehashuevent->next;
	while (hashuevent)
	{
		if (hashuevent == uevent)
		{
			prehashuevent->next = hashuevent->next;

			//关闭的同时会自动将kqueue中的事件去掉
			if (closesock(uevent->fd) == -1)
			{
				return FAILED;
			}

			//释放用户注册事件
			free(hashuevent);

			return SUCCESS;
		}

		prehashuevent = hashuevent;
		hashuevent = hashuevent->next;
	}

	return FAILED;
}

/*获取事件*/
event *getevent(int fd, reactor *reactor)
{
	struct event *hashuevent = gethashevent(fd, reactor);
	while (hashuevent)
	{
		if (hashuevent->fd == fd)
		{
			return hashuevent;
		}

		hashuevent = hashuevent->next;
	}

	return NULL;
}

static int getactevent(reactor *reactor)
{
	//设置默认超时时间
	struct timespec defaulttime = {1, 0};
	struct timespec mintimer = defaulttime;
	if (getminouttimers(reactor, &mintimer) == SUCCESS 
		&& timespeccompare(&defaulttime, &mintimer) == 1)
	{
		memcpy(&mintimer, &defaulttime, sizeof(struct timespec));
	}

	//清空激活链表
	clear(&reactor->uactevelist);

	//获取活动事件
	int actevenum = kevent(reactor->reactorid, NULL, 0, reactor->kevelist, reactor->kevelistlen, &mintimer);
	if (actevenum == -1)
	{
		return FAILED;
	}

	//获取定时器超时事件
	looptimers(reactor);

	//获取活动的用户事件
	for (int i = 0; i < actevenum; i++)
	{
		struct event *hashuevent = getevent(reactor->kevelist[i].ident, reactor);
		if (hashuevent == NULL)
		{
			continue;
		}

		push(&reactor->uactevelist, hashuevent, 0);
		hashuevent->callback(hashuevent->arg);
	}

	return SUCCESS;
}

/*分发消息*/
int dispatchevent(reactor *reactor)
{
	int count = 5;
	while (reactor->listen && (count--))
	{
		if (getactevent(reactor) == FAILED)
		{
			return FAILED;
		}
	}

	return SUCCESS;
}

/*销毁反应堆*/
int destroyreactor(reactor *reactor)
{
	//释放sock事件
	for (int i = 0; i < reactor->uevelistlen; i++)
	{
		struct event *unextevent = NULL;
		struct event *uevent = reactor->uevelist[i].next;
		while (uevent)
		{
			unextevent = uevent->next;
			delevent(uevent);
			uevent = unextevent;
		}
	}
	free(reactor->uevelist);

	//释放计时器事件
	queuenode *headquenode = gethead(&reactor->utimersevelist);
	while (!empty(&reactor->utimersevelist))
	{
		headquenode = gethead(&reactor->utimersevelist);
		deltimer(headquenode->data);
	}

	free(reactor->kevelist);

	destroyqueue(&reactor->uactevelist);
	destroyqueue(&reactor->utimersevelist);

	free(reactor);

	return SUCCESS;
}

// void temp()
// {
// 	//读取数据
// 	if (event.flags & EVFILT_READ)
// 	{
// 		/*接收信息*/
// 		int recvlen = recv(event.ident, uevent.readbuf, READBUF, 0);
// 		if (recvlen == -1 || recvlen == 0)
// 		{
// 			delevent(&uevent);
// 			continue;
// 		}

// 		uevent.readbufsize = recvlen;
// 	}
// 	//写数据
// 	else if (event.flags & EVFILT_WRITE)
// 	{
// 		continue;
// 	}
// 	//错误数据
// 	else if (event.flags & EV_ERROR)
// 	{
// 		delevent(&uevent);
// 		continue;
// 	}
// 	else
// 	{
// 		continue;
// 	}
// }

// for (int i = 0; i < uevent->reactor->uevelistlen; i++)
// 	{	
// 		if (uevent->reactor->uevelist[i].next == NULL)
// 		{
// 			debuginfo("addevent--%d%s", i, "ok");
// 		}
// 	}
