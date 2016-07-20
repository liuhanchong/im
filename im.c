#include "./core/util.h"
#include "./core/net/reactor.h"
#include "./core/net/socket.h"
#include "./pyutil/inface.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

typedef struct im 
{
	int servfd;/*保存服务器套接字*/
	struct reactor *reactor;/*反应堆模型*/
	struct thread *acceptthread;//接收sock线程
} im;

struct im imserv; 

void *readwrite(void *event, void *arg)
{
	struct event *uevent = (struct event *)event;
	if (uevent == NULL)
	{
		return NULL;
	}

	// 	//读取数据
	if (uevent->eventtype & EV_READ)
	{
		/*接收信息*/
		int recvlen = recv(uevent->fd, uevent->readbuf, READBUF - 1, 0);
		if (recvlen <= 0)
		{
			debuginfo("%s->%s failed sockid=%d errno=%d", "readwrite", "recv", uevent->fd, errno);
			if (errno == EINTR)
			{
//				freeevent(uevent);
			}
			
			return NULL;
		}

		uevent->readbufsize = recvlen;
		uevent->readbuf[recvlen] = '\0';

		debuginfo("%s->%s sockid=%d, data=", "readwrite", "recv", uevent->fd);//, uevent->readbuf

		//将客户端套接字注册事件
		struct event *urevent = setevent(uevent->reactor, uevent->fd, EV_WRITE,
		 								readwrite, NULL);
		if (urevent == NULL)
		{
			debuginfo("%s->%s failed clientsock=%d", "readwrite", "setevent", uevent->fd);
			return NULL;
		}

		strncpy((char *)urevent->writebuf, "wo shi server", 13);
		urevent->writebufsize = 13;
		urevent->writebuf[urevent->writebufsize] = '\0';

		if (addevent(urevent) == FAILED)
		{
			debuginfo("%s->%s failed clientsock=%d", "readwrite", "addevent", uevent->fd);
			return NULL;
		}
	}
	else if (uevent->eventtype & EV_WRITE)
	{
		/*发送信息*/
		int sendlen = send(uevent->fd, uevent->writebuf, uevent->writebufsize, 0);
		if (sendlen <= 0)
		{
			debuginfo("%s->%s failed sockid=%d errno=%d", "readwrite", "send", uevent->fd, errno);
			if (errno == EINTR)
			{
//				freeevent(uevent);
			}

			return NULL;
		}

		debuginfo("%s->%s sockid=%d, data=", "readwrite", "send", uevent->fd);//, uevent->writebuf

		//将客户端套接字注册事件
		struct event *uwevent = setevent(uevent->reactor, uevent->fd, EV_READ, readwrite, NULL);
		if (uwevent == NULL)
		{
			debuginfo("%s->%s failed clientsock=%d", "readwrite", "setevent_2", uevent->fd);
			return NULL;
		}

		if (addevent(uwevent) == FAILED)
		{
			debuginfo("%s->%s failed clientsock=%d", "readwrite", "addevent_2", uevent->fd);
			return NULL;
		}
	}

	return NULL;
}

void *eventcallback(void *event, void *arg)
{
//	debuginfo("eventcallback %d", *((int *)arg));

	return NULL;
}

void *timercallback(void *event, void *arg)
{
	debuginfo("timercallback %d", *((int *)arg));

	return NULL;
}

void *closesys(void *event, void *arg)
{
//	debuginfo("closesys");

	struct reactor *reactor = (struct reactor *)arg;
	reactor->listen = 0;

	return NULL;
}

void *signalalam(void *event, void *arg)
{
//	debuginfo("signalalam");

	return NULL;
}

static void *acceptconn(void *uev, void *data)
{
	struct im *im = (struct im *)data;

	int clientsock = acceptsock(im->servfd);
	if (clientsock < 0)
	{
		debuginfo("%s->%s failed clientsock=%d", "acceptconn", "accept", clientsock);
		return NULL;
	}

	//将客户端socket设置为non_blocked
	setnoblock(clientsock);

	//将客户端套接字注册事件
	struct event *uevent = setevent(im->reactor, clientsock, EV_READ | EV_PERSIST, readwrite, NULL);
	if (uevent == NULL)
	{
		debuginfo("%s->%s failed clientsock=%d", "acceptconn", "setevent", clientsock);
		return NULL;
	}

	if (addevent(uevent) == FAILED)
	{
		debuginfo("%s->%s failed clientsock=%d", "acceptconn", "addevent", clientsock);
		return NULL;
	}

	if (addheartbeat(im->reactor->hbeat, clientsock) == SUCCESS)
	{
		debuginfo("%s->%s success clientsock=%d", "acceptconn", "addheartbeat", clientsock);
		return NULL;	
	}
	else
	{
		debuginfo("%s->%s failed clientsock=%d", "acceptconn", "addheartbeat", clientsock);
		return NULL;
	}

	debuginfo("%s->%s sockid=%d success", "acceptconn", "accept", clientsock);

	return NULL;
}

int test1()
{
	char str[100];
	int ret = test(str, 100);
	debuginfo("value = %s", str);
	return ret;
}

int main(int argc, char *argv[])
{
	if (argc == 2 && (strcmp(argv[1], "stop") == 0))
	{
		int pid = getpidfromfile();
		if (pid == -1)
		{
			printf("%s\n", "close pro failed, invalid pid!");
			return 0;
		}

		if (kill(pid, SIGINT) == -1)
		{
			printf("close pro failed, pid=%d!\n", pid);
			return 0;
		}

		printf("close pro success, pid=%d!\n", pid);

		return 0;
	}

	int pid = getpid();
	printf("本进程的ID为 %d\n", pid);
	setpidtofile();

	//2.共享队列互斥处理
	openlog();

	if (setcorefilesize(10 * 1024 * 1024) == SUCCESS)
	{
		debuginfo("set dump core file success size=%d", getcorefilesize());
	}

//	debuginfo("python exe %d", test1());
	sys sysc;
	if (getsyscon("server.ini", &sysc) == SUCCESS)
	{
		debuginfo("ip=%s port=%d", sysc.ip, sysc.port);
	}

	reactor *reactor = NULL;
	if ((reactor = createreactor()) == NULL)
	{
		debuginfo("main->createreactor failed!");
		return 1;
	}
	debuginfo("main->createreactor success");
	imserv.reactor = reactor;

	imserv.servfd = cretcpser((char *)sysc.ip, sysc.port, 10);
	if (imserv.servfd < 0)
	{
		debuginfo("main->cretcpser failed");
		return 1;
	}
	debuginfo("main->cretcpser success");
	imserv.reactor->servfd = imserv.servfd;

	// imserv.acceptthread = createthread(acceptconn, &imserv, 0);
	// if (imserv.acceptthread == NULL)
	// {
	// 	return 1;
	// }
	event *uevent = setevent(reactor, imserv.servfd, EV_READ | EV_PERSIST, acceptconn, &imserv);
	if (uevent == NULL)
	{
		debuginfo("%s->%s failed sersock=%d", "main", "setevent", imserv.servfd);
		return 1;
	}

	if (addevent(uevent) == FAILED)
	{
		debuginfo("%s->%s failed sersock=%d", "main", "addevent", imserv.servfd);
	}
	debuginfo("main->createthread success");

	int i = 100;
	uevent = setevent(reactor, 1, EV_READ | EV_PERSIST, eventcallback, &i);
	if (addevent(uevent) == SUCCESS)
	{
		debuginfo("%s", "addevent ok");
	}

	uevent = setevent(reactor, 2, EV_READ | EV_PERSIST, eventcallback, &i);
	if (addevent(uevent) == SUCCESS)
	{
		debuginfo("%s", "addevent ok");
	}

	struct timespec timer = {1, 0};
	uevent = setevent(reactor, -1, EV_TIMER, timercallback, &i);
	if (addtimer(uevent, &timer) == SUCCESS)
	{
		debuginfo("%s", "addtimer ok");
	}

	int j = 101;
	struct timespec timer2 = {2, 0};
	uevent = setevent(reactor, -1, EV_TIMER | EV_PERSIST, timercallback, &j);
	if (addtimer(uevent, &timer2) == SUCCESS)
	{
		debuginfo("%s", "addtimer ok");
	}

	uevent = setevent(reactor, SIGALRM, EV_SIGNAL | EV_PERSIST, signalalam, reactor);
	if (addsignal(uevent) == SUCCESS)
	{
		debuginfo("%s", "addsignal ok");
	}

	uevent = setevent(reactor, SIGINT, EV_SIGNAL, closesys, reactor);
	if (addsignal(uevent) == SUCCESS)
	{
		debuginfo("%s", "addsignal ok");
	}

	struct itimerval value1;
	value1.it_value.tv_sec = 1;
	value1.it_value.tv_usec = 0;
	value1.it_interval.tv_sec = 1;
	value1.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value1, NULL);

	if (dispatchevent(reactor) == SUCCESS)
	{
		debuginfo("%s", "dispatchevent ok");
	}

	if (destroyreactor(reactor) == SUCCESS)
	{
		debuginfo("%s\n", "reactor ok");
	}

	closelog();

	return 1;
}