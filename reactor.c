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
#include <unistd.h>
#include <errno.h>

/*保存处理信号的reactor*/
static struct reactor *sigreactor = NULL;

/*释放分配的事件*/
int freeevent(struct event *uevent)
{
	if (uevent->eventtype & EV_READ || uevent->eventtype & EV_WRITE)
	{
		//关闭的同时会自动将kqueue中的事件去掉
		if (closesock(uevent->fd) == -1)
		{
			debuginfo("%s->%s sock %d failed", "freeevent", "close", uevent->fd);
		}
	}

	free(uevent);

	return SUCCESS;	
}

static void sighandle(int sigid, siginfo_t *siginfo, void *data)
{
	if (sigreactor == NULL)
	{
		debuginfo("%s->%s null", "sighandle", "sigreactor");
		return;
	}

	sigreactor->sigid = sigid;
	sigreactor->sigstate = 1;

	if (write(sigreactor->sockpair[0], "a", 1) != 1)
	{
		//输出错误日志
		debuginfo("%s->%s failed", "sighandle", "write");
	}
}

/*获取指定的socket事件在hash表位置*/
static struct event *gethashevent(int fd, struct reactor *reactor)
{
	int hashindex = fd % reactor->uevelistlen;
	return &(reactor->uevelist[hashindex]);
}

static int add(struct event *uevent, struct timespec *timer)
{
	if (uevent->eventtype & EV_SIGNAL)
	{
		struct sigaction usignal;
		sigemptyset(&usignal.sa_mask);
		usignal.sa_sigaction = sighandle;
		usignal.sa_flags = SA_SIGINFO | SA_RESTART;
		if (sigaction(uevent->fd, &usignal, &uevent->oldsig) == -1)
		{
			return FAILED;
		}

		//加入到用户事件列表
		int ret = push(&uevent->reactor->usignalevelist, uevent, 0);
		return ret;
	}

	if (uevent->eventtype & EV_TIMER)
	{
		memcpy(&uevent->endtimer, timer, sizeof(struct timespec));
		memcpy(&uevent->intetimer, timer, sizeof(struct timespec));

		//设置计时器时间
		time_t curtime = time(NULL);
		uevent->endtimer.tv_sec += curtime;

		//加入到用户事件列表
		int ret = push(&uevent->reactor->utimersevelist, uevent, 0);
		return ret;
	}

	if (uevent->eventtype & EV_READ || uevent->eventtype & EV_WRITE)
	{
		//获取监听的事件
		int filter = (uevent->eventtype & EV_READ) ? EVFILT_READ : EVFILT_WRITE;

		//设置添加操作
		int flags = (uevent->eventtype & EV_PERSIST) ? EV_ADD : EV_ADD | EV_ONESHOT; 
		
		//加入到系统内核监听
		struct kevent addkevent;
		EV_SET(&addkevent, uevent->fd, filter, flags, 0, 0, NULL);
		if (kevent(uevent->reactor->reactorid, &addkevent, 1, NULL, 0, NULL) == -1)
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

	return FAILED;
}

static int timespeccompare(struct timespec *src, struct timespec *dest)
{
	return (src->tv_sec < dest->tv_sec) ? 1 :
			(src->tv_sec > dest->tv_sec) ? -1 : 
			(src->tv_nsec < dest->tv_nsec) ? 1 : 
			(src->tv_nsec > dest->tv_nsec) ? -1 : 0;
}

static int timercompare(queuenode *src, queuenode *dest)
{
	event *srcuevent = (struct event *)src->data;
	event *destuevent = (struct event *)dest->data;

	return timespeccompare(&srcuevent->endtimer, &destuevent->endtimer);
}

/*获取最小超时时间*/
static int getminouttimers(reactor *reactor, struct timespec *mintimer)
{
	//计算最小超时时间
	queuenode *headquenode = gethead(&reactor->utimersevelist);
	if (headquenode == NULL)
	{
		return FAILED;
	}

	time_t curtime = time(NULL);
	struct timespec headtimer = ((struct event *)headquenode->data)->endtimer;
	if (headtimer.tv_nsec > 0)
	{
		mintimer->tv_nsec = (long)(pow(10, 9) - headtimer.tv_nsec);
		mintimer->tv_sec = headtimer.tv_sec - curtime - 1;
	}
	else
	{
		mintimer->tv_sec = headtimer.tv_sec - curtime;
	}

	//当最小计时器超时
	if (mintimer->tv_sec < 0)
	{
		mintimer->tv_sec = 0;	
		mintimer->tv_nsec = 0;
	}

	return SUCCESS;
}

/*遍历信号事件*/
static int loopsignal(reactor *reactor)
{
	/*没有信号事件*/
	if (reactor->sigstate != 1)
	{
		return SUCCESS;
	}

	reactor->sigstate = 0;/*重置信号状态*/

	queuenode *headquenode = NULL;
	list *looplist = &reactor->usignalevelist;
	looplist(looplist, headquenode)
		struct event *headuevent = (struct event *)headquenode->data;
		if (reactor->sigid == headuevent->fd)
		{
			push(&reactor->uactevelist, headuevent, 0);
		}
	endlooplist(looplist, headquenode)

	return SUCCESS;
}

/*遍历计时器事件*/
static int looptimers(reactor *reactor)
{
	struct timespec curtime = {time(NULL), 0};

	queuenode *headquenode = NULL;
	list *looplist = &reactor->utimersevelist;
	looplist(looplist, headquenode)
		struct event *headuevent = (struct event *)headquenode->data;
		if (timespeccompare(&curtime, &headuevent->endtimer) == 1)
		{
			break;
		}
		push(&reactor->uactevelist, headuevent, 0);
	endlooplist(looplist, headquenode)

	return SUCCESS;
}

/*获取事件*/
static event *getevent(int fd, reactor *reactor)
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
	//清空激活链表
	clear(&reactor->uactevelist);

	//设置默认超时时间
	struct timespec defaulttime = {3, 0};
	struct timespec mintimer = defaulttime;
	if (getminouttimers(reactor, &mintimer) == SUCCESS 
		&& timespeccompare(&defaulttime, &mintimer) == 1)
	{
		memcpy(&mintimer, &defaulttime, sizeof(struct timespec));
	}

	//获取活动事件
	int actevenum = kevent(reactor->reactorid, NULL, 0,
							 reactor->kevelist, reactor->kevelistlen, &mintimer);
	if (actevenum == -1)
	{
		//对于信号中断不做处理
		if (errno != EINTR)
		{
			debuginfo("%s->%s failed no:%d", "getactevent", "actevenum", errno);
			return FAILED;
		}
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

		//对于来到的信号事件
		if (hashuevent->fd == reactor->sockpair[1])
		{
			//需要清空缓冲区
			char array[2];
			int readlen = read(hashuevent->fd, array, 2);
			array[readlen] = '\0';

			//获取信号事件
			loopsignal(reactor);
		}
		//普通的读写事件
		else
		{
			push(&reactor->uactevelist, hashuevent, 0);
		}
	}

	return SUCCESS;
}

/*处理事件*/
static int disposeevent(reactor *reactor)
{
	queuenode *headquenode = NULL;
	looplist(&reactor->uactevelist, headquenode)
		event *uevent = (event *)headquenode->data;
		//定时器事件
		if (uevent->eventtype & EV_TIMER)
		{
			if (uevent->eventtype & EV_PERSIST)
			{
				uevent->endtimer.tv_sec = time(NULL) + uevent->intetimer.tv_sec;
			}
		}
		//信号事件
		else if (uevent->eventtype & EV_SIGNAL)
		{
		}
		//读写事件
		else if (uevent->eventtype & EV_READ || uevent->eventtype & EV_WRITE)
		{
		}

		//统一处理事件
		uevent->callback(uevent, uevent->arg);
		if (!(uevent->eventtype & EV_PERSIST))
		{
			delevent(uevent);
			debuginfo("delete event, this event not is EV_PERSIST event");
		}
	endlooplist(&reactor->uactevelist, headquenode)

	return SUCCESS;
}

/*创建反应堆*/
struct reactor *createreactor()
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
	debuginfo("%s->%s maxfileno=%d", "createreactor", "getmaxfilenumber", evenumber);
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

	pthread_mutexattr_t mutexattr;
	if (pthread_mutexattr_init(&mutexattr) != 0 ||
		pthread_mutex_init(&newreactor->uevelistmutex, &mutexattr) != 0)
	{
		return NULL;
	}

	newreactor->kevelistlen = evenumber;
	newreactor->kevelist = (struct kevent *)malloc(sizeof(struct kevent) * evenumber);
	if (newreactor->kevelist == NULL)
	{
		return NULL;
	}

	/*活动链表*/
	if (createqueue(&newreactor->uactevelist, 0, 0, NULL) == FAILED)
	{
		return NULL;
	}

	/*计时器链表*/
	if (createqueue(&newreactor->utimersevelist, 0, 2, timercompare) == FAILED)
	{
		return NULL;
	}

	/*信号链表*/
	if (createqueue(&newreactor->usignalevelist, 0, 0, NULL) == FAILED)
	{
		return NULL;
	}

	/*开启监听事件*/
	newreactor->listen = 1;

	/*初始化信号*/
	newreactor->sigstate = 0;
	newreactor->sigid = 0;

	//创建sock对 并注册读事件
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, newreactor->sockpair) == -1)
	{
		return NULL;
	}

	event *uevent = setevent(newreactor, newreactor->sockpair[1], EV_READ | EV_PERSIST, NULL, NULL);
	if (uevent == NULL)
	{
		return NULL;
	}

	if (addevent(uevent) == FAILED)
	{
		return NULL;
	}
	
	return newreactor;
}

/*设置事件*/
struct event *setevent(struct reactor *reactor, int fd, int evetype, void *(*callback)(void *, void *), void *arg)
{
	//创建一个事件
	struct event *newevent = malloc(sizeof(struct event));
	if (newevent == NULL)
	{
		return NULL;
	}
	memset(newevent, 0, sizeof(struct event));

	newevent->fd = fd;
	newevent->eventtype = evetype;
	newevent->callback = callback;
	newevent->arg = arg;
	newevent->reactor = reactor;

	return newevent;
}

/*添加信号事件*/
int addsignal(struct event *uevent)
{
	/*信号只对于一个反应堆起作用*/
	sigreactor = uevent->reactor;
	return add(uevent, NULL);
}

/*添加计时器事件*/
int addtimer(struct event *uevent, struct timespec *timer)
{
	return add(uevent, timer);
}

/*添加事件*/
int addevent(event *uevent)
{
	return add(uevent, NULL);
}

int delevent(event *uevent)
{
	if ((uevent->eventtype & EV_SIGNAL) || (uevent->eventtype & EV_TIMER))
	{
		struct list *looplist = (uevent->eventtype & EV_SIGNAL) ? 
									&uevent->reactor->usignalevelist : &uevent->reactor->utimersevelist;

		queuenode *headquenode = NULL;
		looplist(looplist, headquenode)
			struct event *headuevent = (struct event *)headquenode->data;
		 // 针对信号 if (uevent->sigid == headuevent->sigid)
		 // 针对计时器 if (uevent->callback == headuevent->callback 
		 // 	&& (timespeccompare(&uevent->timer, &headuevent->timer) == 0))
			if (headuevent == uevent)
			{
//				free(uevent);
				delete(looplist, headquenode);
				return SUCCESS;
			}
		endlooplist(looplist, headquenode)

		return FAILED;
	}

	if (uevent->eventtype & EV_READ || uevent->eventtype & EV_WRITE)
	{
		struct event *prehashuevent = gethashevent(uevent->fd, uevent->reactor);
		struct event *hashuevent = prehashuevent->next;
		while (hashuevent)
		{
			if (hashuevent == uevent)
			{
				prehashuevent->next = hashuevent->next;

				//关闭的同时会自动将kqueue中的事件去掉
				// if (closesock(uevent->fd) == -1)
				// {
				// 	return FAILED;
				// }

				// //释放用户注册事件
				// free(hashuevent);

				return SUCCESS;
			}

			prehashuevent = hashuevent;
			hashuevent = hashuevent->next;
		}

		return FAILED;
	}

	return FAILED;
}

/*分发消息*/
int dispatchevent(reactor *reactor)
{
	while (reactor->listen)
	{
		if (getactevent(reactor) == FAILED)
		{
			return FAILED;
		}

		if (disposeevent(reactor) == FAILED)
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
			freeevent(uevent);
			uevent = unextevent;
		}
	}
	free(reactor->uevelist);

	pthread_mutex_destroy(&reactor->uevelistmutex);

	//释放计时器事件
	queuenode *headquenode = NULL;
	while (!empty(&reactor->utimersevelist))
	{
		headquenode = gethead(&reactor->utimersevelist);
		delevent(headquenode->data);
		freeevent(headquenode->data);
	}

	//释放信号事件
	while (!empty(&reactor->usignalevelist))
	{
		headquenode = gethead(&reactor->usignalevelist);
		delevent(headquenode->data);
		freeevent(headquenode->data);
	}

	free(reactor->kevelist);

	destroyqueue(&reactor->uactevelist);
	destroyqueue(&reactor->utimersevelist);
	destroyqueue(&reactor->usignalevelist);

	//关闭sock对
	close(reactor->sockpair[0]);
//	close(reactor->sockpair[1]);

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
