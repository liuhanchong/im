#ifndef REACTOR_H
#define REACTOR_H

#include "queue.h"
#include <time.h>
#include <signal.h>

/*网络字节缓冲区*/
#define READBUF  1024
#define WRITEBUF 1024

/*反应堆支持的事件类型*/
#define EV_TIMEOUT 0x01 //定时器事件
#define EV_READ 0x02    //读取事件
#define EV_WRITE 0x04   //写入事件
#define EV_SIGNAL 0x08  //信号事件
#define EV_PERSIST 0x10 //持久事件
#define EV_ET 0x20 //边缘触发模式

typedef struct reactor
{
	int reactorid;
	struct event *uevelist;/*用户注册事件列表*/
	int uevelistlen;/*保存用户注册散列表长度*/
	pthread_mutex_t uevelistmutex;/*用户事件列表互斥锁*/
	list uactevelist;/*用户注册的活动事件列表*/
	list utimersevelist;/*用户注册的计时器列表*/
	int sigid;/*注册信号类型*/
	int sigstate;/*注册事件状态 0-未触发 1-触发*/
	list usignalevelist;/*用户注册的信号事件列表*/
	struct kevent *kevelist;/*系统内核事件列表*/
	int kevelistlen;/*系统注册时间列表*/
	int listen;//是否监听事件
	int sockpair[2];//sockpair对，用于信号触发
} reactor;

typedef struct event
{
	int fd;/*对于定时器事件-1*/
	int sigid;/*注册信号类型*/
	int eventflag;/*读写、计时器-1、信号-2等事件*/
	void *(*callback)(void *);
	void *arg;
	reactor *reactor;/*所属反应器*/
	unsigned char readbuf[READBUF];
	int readbufsize;
	unsigned char writebuf[WRITEBUF];
	int writebufsize;
	struct timespec timer;/*定时器时间*/
	struct timespec tmtimer;/*保存定时间执行间隔*/
	struct sigaction oldsig;/*保存老的信号处理*/
	struct event *next; 
} event;

/*设置信号事件*/
event *setsignal(reactor *reactor, void *(*callback)(void *), void *arg);

/*添加信号事件*/
int addsignal(event *uevent, int sigid, int eventflag);

/*删除信号事件*/
int delsignal(event *uevent);

/*设置计时器事件*/
event *settimer(reactor *reactor, void *(*callback)(void *), void *arg, struct timespec *timer);

/*添加计时器事件*/
int addtimer(event *uevent, int eventflag);

/*删除计时器事件*/
int deltimer(event *uevent);

/*创建反应堆*/
reactor *createreactor();

/*设置事件*/
event *setevent(reactor *reactor, int fd, void *(*callback)(void *), void *arg);

/*添加事件*/
int addevent(event *uevent, int eventflag);

/*删除事件*/
int delevent(event *uevent);

/*分发消息*/
int dispatchevent(reactor *reactor);

/*销毁反应堆*/
int destroyreactor(reactor *reactor);

#endif