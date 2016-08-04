#include "./core/util.h"
#include "./core/net/reactor.h"
#include "./core/net/socket.h"
#include "./pyutil/inface.h"
#include "./work/datainface.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>

struct im imserv; 

void *eventcallback(void *event, void *arg)
{
	debuginfo("eventcallback %d", *((int *)arg));

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
	debuginfo("signalalam");

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

	printf("the process id is %d!\n", getpid());

	setpidtofile();

	//2.共享队列互斥处理
	openlog();

	debuginfo("the system cpu core num is %d", getcpucorenum());

	if (setcorefilesize(100 * 1024 * 1024) == SUCCESS)
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
	debuginfo("main->cretcpser success serid=%d", imserv.servfd);
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
	// uevent = setevent(reactor, 1, EV_READ | EV_PERSIST, eventcallback, &i);
	// if (addevent(uevent) == SUCCESS)
	// {
	// 	debuginfo("%s", "addevent ok");
	// }

	// uevent = setevent(reactor, 2, EV_READ | EV_PERSIST, eventcallback, &i);
	// if (addevent(uevent) == SUCCESS)
	// {
	// 	debuginfo("%s", "addevent ok");
	// }

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
		debuginfo("%s", "reactor ok");
	}

	debuginfo("%s", "close server success");

	closelog();

	return 1;
}