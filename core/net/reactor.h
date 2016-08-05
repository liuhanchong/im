#ifndef REACTOR_H
#define REACTOR_H

#include "rinface.h"
#include "hbeat.h"

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

/*分发消息*/
int dispatchevent(reactor *reactor);

/*销毁反应堆*/
int destroyreactor(reactor *reactor);

/*释放分配的事件*/
int freeevent(struct event *uevent);

/*删除事件拓展函数*/
int freeeventex(int fd, reactor *reactor);

#endif