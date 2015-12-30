#ifndef REACTOR_H
#define REACTOR_H

#define READBUF  1024
#define WRITEBUF 1024

typedef struct reactor
{
	int reactorid;
	void *evelist;/*事件列表*/
	void *eveactivelist;/*活动事件列表*/
	int evenumber;/*事件个数*/
	int eveactivenumber;/*活动事件的个数*/
} reactor;

typedef struct event
{
	int fd;
	int eventflag;
	void *(*callback)(void *);
	void *arg;
	reactor *reactor;
	unsigned char readbuf[READBUF];
	int readbufsize;
	unsigned char writebuf[WRITEBUF];
	int writebufsize;
} event;

/*创建反应堆*/
int createreactor(int evenumber);

/*设置事件*/
int setevent(int fd, void *(*callback)(void *), void *arg);

/*添加事件*/
int addevent(reactor *reactor, event *event, int eventflag);

/*修改事件*/
int modifyevent(event *event, int eventflag, void *(*callback)(void *), void *arg);

/*删除事件*/
int delevent(event *event);

/*分发消息*/
int dispatchevent(reactor *reactor);

#endif