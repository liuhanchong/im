#ifndef REACTOR_H
#define REACTOR_H

#include "../queue.h"
#include "../htable.h"
#include "hbeat.h"
#include <time.h>
#include <signal.h>

/*网络字节缓冲区*/
#define READBUF  1024
#define WRITEBUF 1024

/*反应堆支持的事件类型*/
#define EV_TIMER 0x01 //定时器事件
#define EV_READ 0x02    //读取事件
#define EV_WRITE 0x04   //写入事件
#define EV_SIGNAL 0x08  //信号事件
#define EV_PERSIST 0x10 //持久事件
#define EV_ET 0x20 //边缘触发模式

/*信号数量*/
#define SIGNUM NSIG

typedef struct signalevent
{
	int sigid[SIGNUM];/*注册信号类型*/
	int sigstate;/*注册事件状态 0-未触发 1-触发*/
	list usignalevelist;/*用户注册的信号事件列表*/
	int sockpair[2];/*sockpair对，用于信号触发*/
} signalevent;

typedef struct reactor
{
	int reactorid;
	pthread_mutex_t reactormutex;/*反应堆锁*/
	hashtable *uevelist;/*用户注册读写事件hash列表*/
	list uactevelist;/*用户注册的活动事件列表*/
	list utimersevelist;/*用户注册的计时器列表*/
	signalevent usigevelist;/*用户注册的信号事件列表*/
	struct kevent *kevelist;/*系统内核事件列表*/
	int kevelistlen;/*系统注册时间列表*/
	int listen;/*是否监听事件*/
	int servfd;/*服务端套接字*/
	int maxconnnum;/*保存最大的连接数*/
	struct heartbeat *hbeat;/*心跳管理*/
} reactor;

typedef struct event
{
	int fd;/*计时器:-1 信号:信号值 IO:文件描述符*/
	int eventtype;/*读写、计时器、信号*/
	void *(*callback)(void *, void *);
	void *arg;
	reactor *reactor;/*所属反应器*/
	unsigned char readbuf[READBUF];
	int readbufsize;
	unsigned char writebuf[WRITEBUF];
	int writebufsize;
	struct timespec endtimer;/*保存定时器结束时间*/
	struct timespec intetimer;/*保存定时间隔*/
	struct sigaction *oldsiga;/*保存设置前信号处理*/
	struct event *next; 
} event;

/*创建反应堆*/
struct reactor *createreactor();

/*设置事件*/
struct event *setevent(struct reactor *reactor, int fd, int evetype,
						 void *(*callback)(void *, void *), void *arg);

/*添加信号事件*/
int addsignal(struct event *uevent);

/*添加计时器事件*/
int addtimer(event *uevent, struct timespec *timer);

/*添加事件*/
int addevent(event *uevent);

/*删除事件*/
int delevent(event *uevent);

/*删除事件拓展函数*/
int freeevent_ex(int fd, reactor *reactor);

/*分发消息*/
int dispatchevent(reactor *reactor);

/*销毁反应堆*/
int destroyreactor(reactor *reactor);

/*释放分配的事件*/
int freeevent(struct event *uevent);

#endif