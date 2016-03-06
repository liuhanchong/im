#include "reactor.h"
#include "common.h"
#include "socket.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <string.h>

/*创建反应堆*/
int createreactor(reactor *reactor)
{
	int evenumber = 1024;/*此处应该使用进程最大打开文件数目*/

	memset(reactor, 0, sizeof(struct reactor));

	reactor->reactorid = kqueue();
	if (reactor->reactorid == -1)
	{
		return FAILED;
	}
	
	int evelistsize = sizeof(struct event *) * evenumber;
	reactor->evenumber = evenumber;
	*reactor->evelist = (void *)malloc(evelistsize);
	if (*reactor->evelist == NULL)
	{
		return FAILED;
	}
	memset(*reactor->evelist, 0, evelistsize);
	
	*reactor->eveactivelist = NULL;
	reactor->eveactivenumber = 0;

	reactor->timerslist = NULL;
	reactor->timersnumber = 0;

	reactor->kevelist = (struct kevent *)malloc(sizeof(struct kevent) * evenumber);
	if (reactor->kevelist == NULL)
	{
		return FAILED;
	}
	
	return SUCCESS;
}

/*设置事件*/
void setevent(event *event, reactor *reactor, int fd, void *(*callback)(void *), void *arg)
{
	event->fd = fd;
	event->callback = callback;
	event->arg = arg;
	event->reactor = reactor;
	event->readbufsize = 0;
	event->writebufsize = 0;
	event->next = NULL;
}

/*添加事件*/
int addevent(event *event, int eventflag)
{
	event->eventflag = eventflag;
	
	//加入到系统内核监听
	struct kevent addevent;
	EV_SET(&addevent, event->fd, eventflag, EV_ADD, 0, 0, 0);
	if (kevent(event->reactor->reactorid, &addevent, 1, NULL, 0, NULL) == -1)
	{
		return FAILED;
	}

	//加入到用户事件列表
	int hashindex = event->fd % event->reactor->evenumber;
	struct event **hashevent = (struct event **)&(event->reactor->evelist[hashindex]);
	if (*hashevent == NULL)
	{
		*hashevent = event;
	}
	else
	{
		while ((*hashevent)->next)
		{
			*hashevent = (*hashevent)->next;
		}
		(*hashevent)->next = event;
	}
	
	return SUCCESS;
}

/*删除事件*/
int delevent(event *event)
{
	int hashindex = event->fd % event->reactor->evenumber;
	struct event *hashevent = (struct event *)event->reactor->evelist[hashindex];

	struct event **headevent = &hashevent; 
	struct event *preevent = hashevent;
	while (hashevent)
	{
		if (hashevent->fd == event->fd)
		{
			struct kevent delevent;
			EV_SET(&delevent, event->fd, event->eventflag, EV_DELETE, 0, 0, 0);
			if (kevent(event->reactor->reactorid, &delevent, 1, NULL, 0, NULL) == -1)
			{
				return FAILED;
			}

			if (closesock(event->fd) == -1)
			{
				return FAILED;
			}

			if (hashevent == *headevent)
			{
				*headevent = hashevent->next;
			}
			else
			{
				preevent->next = hashevent->next;
			}
			free(hashevent);
			hashevent = NULL;

			return SUCCESS;
		}

		preevent = hashevent;
		hashevent = hashevent->next;
	}

	return FAILED;
}

/*获取事件*/
int getevent(event *event)
{
	int hashindex = event->fd % event->reactor->evenumber;
	struct event *hashevent = (struct event *)event->reactor->evelist[hashindex];

	while (hashevent)
	{
		if (hashevent->fd == event->fd)
		{
			memcpy(event, hashevent, sizeof(struct event));
			return SUCCESS;
		}

		hashevent = hashevent->next;
	}

	return FAILED;
}

/*分发消息*/
int dispatchevent(reactor *reactor)
{
	/*五秒超时*/
	struct timespec time = {.tv_sec = 5, .tv_nsec = 0};
	int activenumber = kevent(reactor->reactorid, NULL, 0,
							 reactor->kevelist, reactor->evenumber, &time);
	if (activenumber == -1)
	{
		return FAILED;
	}

	struct kevent event;
	for (int i = 0; i < activenumber; i++)
	{
		event = reactor->kevelist[i];

		struct event uevent;
		uevent.fd = event.ident;
		if (getevent(&uevent) == FAILED)
		{
			continue;
		}
	}

	return SUCCESS;
}

/*销毁反应堆*/
int destroyreactor(reactor *reactor)
{
	free(*reactor->evelist);
	free(*reactor->eveactivelist);
	free(reactor->kevelist);

	struct event *timer = (struct event *)reactor->timerslist;
	struct event *next = NULL;
	while (timer)
	{
		next = timer->next;
		free(timer);
		timer = next;
	}

	memset(reactor, 0, sizeof(struct reactor));

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
