#include "reactor.h"
#include "../util.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

/*保存处理信号的reactor*/
static struct reactor *sigreactor = NULL;

static void *clearevsig(void *event, void *arg)
{
	assert((event != NULL));

	struct event *uevent = (struct event *)event;

	//对于来到的信号事件
	if (uevent->fd == uevent->reactor->usigevelist.sockpair[1])
	{
		//需要清空缓冲区
		char array[2];
		int readlen = recv(uevent->fd, array, 2, 0);
	}

	return NULL;
}

static void sighandle(int sigid, siginfo_t *siginfo, void *data)
{
	if (sigreactor == NULL)
	{
		debuginfo("%s->%s null", "sighandle", "sigreactor");
		return;
	}

	sigreactor->usigevelist.sigid[sigid] = sigid;
	sigreactor->usigevelist.sigstate = 1;

	if (send(sigreactor->usigevelist.sockpair[0], "a", 1, 0) != 1)
	{
		//输出错误日志
		debuginfo("%s->%s failed", "sighandle", "write");
	}
}

/*获取指定的socket事件在hash表位置*/
static struct event *gethashevent(int fd, struct reactor *reactor)
{
	int hashindex = fd % reactor->uevelist.uevelistlen;
	return &(reactor->uevelist.uevehashtable[hashindex]);
}

/*获取事件*/
static event *getevent(int fd, reactor *reactor)
{
	assert((reactor != NULL && fd >= 0));

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

static int add(struct event *uevent, struct timespec *timer)
{
	assert((uevent != NULL));

	if (uevent->eventtype & EV_SIGNAL)
	{
		/*注册信号*/
		struct sigaction usignal;
		sigemptyset(&usignal.sa_mask);
		usignal.sa_sigaction = sighandle;
		usignal.sa_flags = SA_SIGINFO | SA_RESTART;
		if (sigaction(uevent->fd, &usignal, &uevent->oldsig) == -1)
		{
			return FAILED;
		}

		//加入到用户事件列表
		int ret = push(&uevent->reactor->usigevelist.usignalevelist, uevent, 0);
		return ret;
	}

	if (uevent->eventtype & EV_TIMER)
	{
		assert((timer != NULL));

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

		/*删除之前重复注册过的事件*/
		struct event *uexevent = getevent(uevent->fd, uevent->reactor);
		if (uexevent)
		{
			if (delevent(uexevent) == FAILED)
			{
				debuginfo("%s->%s failed clientsock=%d", "add", "delevent", uevent->fd);
			}
			else
			{
				debuginfo("del exist event success clientsock=%d, filter=%s",
				 uevent->fd, (uevent->eventtype & EV_READ) ? "EVFILT_READ" : "EVFILT_WRITE");
			}
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
	assert((reactor != NULL && mintimer != NULL));

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
	assert((reactor != NULL));

	/*没有信号事件*/
	if (reactor->usigevelist.sigstate != 1)
	{
		return SUCCESS;
	}

	reactor->usigevelist.sigstate = 0;/*重置信号状态*/

	int sigid = 0;
	for (int i = 1; i < NSIG; ++i)
	{
		if ((sigid = reactor->usigevelist.sigid[i]) == 0)
		{
			continue;
		}
		reactor->usigevelist.sigid[i] = 0;
		looplist_for(reactor->usigevelist.usignalevelist)
		{
			struct event *headuevent = (struct event *)headquenode->data;
			if (sigid == headuevent->fd)
			{
				push(&reactor->uactevelist, headuevent, 0);
			}
		}
	}

	return SUCCESS;
}

/*遍历计时器事件*/
static int looptimers(reactor *reactor)
{
	assert((reactor != NULL));

	struct timespec curtime = {time(NULL), 0};

	looplist_for(reactor->utimersevelist)
	{
		struct event *headuevent = (struct event *)headquenode->data;
		if (timespeccompare(&curtime, &headuevent->endtimer) == 1)
		{
			break;
		}
		push(&reactor->uactevelist, headuevent, 0);
	}

	return SUCCESS;
}

static int getactiveevent(reactor *reactor)
{
	assert((reactor != NULL));

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

		//获取信号事件
		loopsignal(reactor);
	}

	//获取定时器超时事件
	looptimers(reactor);

	if (reactor->usigevelist.sigstate)
	{
		//获取信号事件
		loopsignal(reactor);
	}

	//获取活动的用户事件
	for (int i = 0; i < actevenum; i++)
	{
		struct event *hashuevent = getevent(reactor->kevelist[i].ident, reactor);
		if (hashuevent == NULL)
		{
			continue;
		}

		push(&reactor->uactevelist, hashuevent, 0);
	}

	return SUCCESS;
}

/*处理事件*/
static int handleevent(reactor *reactor)
{
	assert((reactor != NULL));

	looplist_for(reactor->uactevelist)
	{
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
		//读事件
		else if (uevent->eventtype & EV_READ)
		{
			if (upheartbeat(uevent->reactor->hbeat, uevent->fd) == SUCCESS)
			{
//				errorinfo("%s->%s success clientsock=%d", "handleevent", "upheartbeat", uevent->fd);
			}
		}
		//写事件
		else if (uevent->eventtype & EV_WRITE)
		{
		}

		//统一处理事件
		uevent->callback(uevent, uevent->arg);

		//将非持久事件删除		
		if (!(uevent->eventtype & EV_PERSIST))
		{
			delevent(uevent);
			debuginfo("delete event, this event not is EV_PERSIST event");
		}
	}

	return SUCCESS;
}

/*创建反应堆*/
struct reactor *createreactor()
{
	reactor *newreactor = (reactor *)malloc(sizeof(reactor));
	if (newreactor == NULL)
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

	/*初始化心跳管理*/
	newreactor->hbeat = createheartbeat(evenumber, 10);
	if (!newreactor->hbeat)
	{
		free(newreactor);
		return NULL;
	}
	newreactor->hbeat->reactor = newreactor;

	newreactor->reactorid = kqueue();
	if (newreactor->reactorid == -1)
	{
		free(newreactor);
		return NULL;
	}

	/*保存最大连接数*/
	newreactor->maxconnnum = evenumber;

	/*初始化hash表 链地址法处理冲突*/
	newreactor->uevelist.uevelistlen = evenumber;
	int uevelistsize = sizeof(struct event) * evenumber;
	newreactor->uevelist.uevehashtable = (struct event *)malloc(uevelistsize);
	if (newreactor->uevelist.uevehashtable == NULL)
	{
		free(newreactor);
		return NULL;
	}
	memset(newreactor->uevelist.uevehashtable, 0, uevelistsize);

	pthread_mutexattr_t mutexattr;
	if (pthread_mutexattr_init(&mutexattr) != 0 ||
		pthread_mutex_init(&newreactor->reactormutex, &mutexattr) != 0 ||
		pthread_mutex_init(&newreactor->uevelist.uevelistmutex, &mutexattr) != 0)
	{
		free(newreactor->uevelist.uevehashtable);
		free(newreactor);
		return NULL;
	}

	newreactor->kevelistlen = evenumber;
	newreactor->kevelist = (struct kevent *)malloc(sizeof(struct kevent) * evenumber);
	if (newreactor->kevelist == NULL)
	{
		free(newreactor->uevelist.uevehashtable);
		free(newreactor);
		return NULL;
	}

	/*活动链表*/
	if (createqueue(&newreactor->uactevelist, 0, 0, NULL) == FAILED)
	{
		destroyreactor(newreactor);
		return NULL;
	}

	/*计时器链表*/
	if (createqueue(&newreactor->utimersevelist, 0, 2, timercompare) == FAILED)
	{
		destroyreactor(newreactor);
		return NULL;
	}

	/*信号链表*/
	if (createqueue(&newreactor->usigevelist.usignalevelist, 0, 0, NULL) == FAILED)
	{
		destroyreactor(newreactor);
		return NULL;
	}

	/*开启监听事件*/
	newreactor->listen = 1;

	/*初始化信号*/
	newreactor->usigevelist.sigstate = 0;
	memset(newreactor->usigevelist.sigid, 0, sizeof(newreactor->usigevelist.sigid));

	//创建sock对 并注册读事件
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, newreactor->usigevelist.sockpair) == -1)
	{
		destroyreactor(newreactor);
		return NULL;
	}

	event *uevent = setevent(newreactor, newreactor->usigevelist.sockpair[1],
					 		EV_READ | EV_PERSIST, clearevsig, NULL);
	if (uevent == NULL)
	{
		destroyreactor(newreactor);
		return NULL;
	}

	if (addevent(uevent) == FAILED)
	{
		destroyreactor(newreactor);
		return NULL;
	}
	
	return newreactor;
}

/*设置事件*/
struct event *setevent(struct reactor *reactor, int fd, int evetype, void *(*callback)(void *, void *), void *arg)
{
	assert((reactor != NULL));

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
	assert((uevent != NULL));

	/*信号只对于一个反应堆起作用*/
	sigreactor = uevent->reactor;
	return add(uevent, NULL);
}

/*添加计时器事件*/
int addtimer(struct event *uevent, struct timespec *timer)
{
	assert((uevent != NULL && timer != NULL));

	return add(uevent, timer);
}

/*添加事件*/
int addevent(event *uevent)
{
	assert((uevent != NULL));

	return add(uevent, NULL);
}

int delevent(event *uevent)
{
	assert((uevent != NULL));

	if ((uevent->eventtype & EV_SIGNAL) || (uevent->eventtype & EV_TIMER))
	{
		struct list *looplist = (uevent->eventtype & EV_SIGNAL) ? 
									&uevent->reactor->usigevelist.usignalevelist : 
									&uevent->reactor->utimersevelist;

		looplist_for(*looplist)
		{
			struct event *headuevent = (struct event *)headquenode->data;
			if (headuevent == uevent)
			{
				delete(looplist, headquenode);
				return SUCCESS;
			}
		}

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
	assert((reactor != NULL));

	while (reactor->listen)
	{
		if (getactiveevent(reactor) == FAILED)
		{
			return FAILED;
		}

		if (handleevent(reactor) == FAILED)
		{
			return FAILED;
		}
	}

	return SUCCESS;
}

/*销毁反应堆*/
int destroyreactor(reactor *reactor)
{
	assert((reactor != NULL));

	destroyheartbeat(reactor->hbeat);

	//获取到信号的pair读事件并从hash表删除
	struct event *pairevent = getevent(reactor->usigevelist.sockpair[1], reactor);
	delevent(pairevent);

	//释放sock事件
	for (int i = 0; i < reactor->uevelist.uevelistlen; i++)
	{
		struct event *unextevent = NULL;
		struct event *uevent = reactor->uevelist.uevehashtable[i].next;
		while (uevent)
		{
			unextevent = uevent->next;
			freeevent(uevent);
			uevent = unextevent;
		}
	}
	free(reactor->uevelist.uevehashtable);

	pthread_mutex_destroy(&reactor->uevelist.uevelistmutex);

	//释放计时器事件
	queuenode *headquenode = NULL;
	while (!empty(&reactor->utimersevelist))
	{
		headquenode = gethead(&reactor->utimersevelist);
		freeevent(headquenode->data);
	}

	//释放信号事件
	while (!empty(&reactor->usigevelist.usignalevelist))
	{
		headquenode = gethead(&reactor->usigevelist.usignalevelist);
		freeevent(headquenode->data);
	}

	free(reactor->kevelist);

	destroyqueue(&reactor->uactevelist);
	destroyqueue(&reactor->utimersevelist);
	destroyqueue(&reactor->usigevelist.usignalevelist);

	//关闭sock对
	close(reactor->usigevelist.sockpair[0]);

	pthread_mutex_destroy(&reactor->reactormutex);

	free(reactor);

	return SUCCESS;
}

/*释放分配的事件*/
int freeevent(struct event *uevent)
{
	assert((uevent != NULL));
	
	delevent(uevent);

	if (uevent->eventtype & EV_READ || uevent->eventtype & EV_WRITE)
	{
		//关闭的同时会自动将kqueue中的事件去掉
		if (delheartbeat(uevent->reactor->hbeat, uevent->fd) == SUCCESS)
		{
			errorinfo("%s->%s success clientsock=%d", "freeevent", "delheartbeat", uevent->fd);
		}

		if (close(uevent->fd) == -1)
		{
			debuginfo("%s->%s sock %d failed", "freeevent", "close", uevent->fd);
		}
	}
	else if (uevent->eventtype & EV_SIGNAL)
	{
		if (!(uevent->fd > 0 && uevent->fd <= NSIG) ||
			sigaction(uevent->fd, &uevent->oldsig, NULL) == -1)
		{
			debuginfo("%s->%s sigid %d failed", "freeevent", "sigaction", uevent->fd);
		}
	}

	free(uevent);

	return SUCCESS;	
}

/*删除事件拓展函数*/
int freeevent_ex(int fd, reactor *reactor)
{
	event *uevent = getevent(fd, reactor);
	if (!uevent)
	{
		return FAILED;
	}

	return freeevent(uevent);
}
