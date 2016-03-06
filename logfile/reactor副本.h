#ifndef REACTOR_H
#define REACTOR_H

#include "queue.h"
#include <time.h>

#define READBUF  1024
#define WRITEBUF 1024

typedef struct reactor
{
	int reactorid;
	void **evelist;/*事件列表*/
	void **eveactivelist;/*活动事件列表*/
	int evenumber;/*事件个数*/
	int eveactivenumber;/*活动事件的个数*/
	struct kevent *kevelist;/*系统内核事件列表*/
	void *timerslist;/*计时器列表*/
	int timersnumber;/*计时器个数*/
} reactor;

typedef struct event
{
	int fd;/*对于定时器事件-1*/
	int eventflag;
	void *(*callback)(void *);
	void *arg;
	reactor *reactor;
	unsigned char readbuf[READBUF];
	int readbufsize;
	unsigned char writebuf[WRITEBUF];
	int writebufsize;
	struct timespec timer;
	struct event *next; 
} event;

/*创建反应堆*/
int createreactor(reactor *reactor);

/*设置事件*/
void setevent(event *event, reactor *reactor, int fd, void *(*callback)(void *), void *arg);

/*添加事件*/
int addevent(event *event, int eventflag);

/*删除事件*/
int delevent(event *event);

/*获取事件*/
int getevent(event *event);

/*分发消息*/
int dispatchevent(reactor *reactor);

/*销毁反应堆*/
int destroyreactor(reactor *reactor);

#endif