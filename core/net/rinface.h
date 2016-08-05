#ifndef RINFACE_H
#define RINFACE_H

#include "../queue.h"
#include "../htable.h"
#include <time.h>
#include <signal.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

/*选择事件类型*/
#if defined (_WIN32)
	#error no support operate system

#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/event.h>
	#define eventt kevent
	#define evset EV_SET

#elif defined(__linux__) || defined(_AIX) || defined(__FreeBSD__)  
#include <sys/epoll.h>
	#define eventt epoll_event
	#define evset(event, f, filter) \
			{ \
				(event)->data.fd = f; \
				(event)->events = filter; \
			}

#else
	#error no support operate system
#endif  

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
#define EV_CTL_ADD 0x40 //添加
#define EV_CTL_MOD 0x80 //修改
#define EV_CTL_DEL 0x100 //删除

/*信号数量*/
#define SIGNUM 65

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
	struct eventt *kevelist;/*系统内核事件列表*/
	int kevelistlen;/*系统注册时间列表*/
	int listen;/*是否监听事件*/
	int servfd;/*服务端套接字*/
	int maxconnnum;/*保存最大的连接数*/
	struct heartbeat *hbeat;/*心跳管理*/
	struct timespec defaulttime;/*默认监听超时*/
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

int cre(int num);
int ctlev(struct event *uevent);
int waitev(struct reactor *reactor, struct timespec *ts);
int clo(int fd);
int getfd(struct eventt *uevent);

#endif