#include "datainface.h"
extern "C"
{
	#include "../core/util.h"
	#include "../core/net/reactor.h"
	#include "../core/net/socket.h"
}
#include "handlereq.h"
#include <errno.h>
#include <string.h>

void *acceptconn(void *uev, void *data)
{
	struct im *im = (struct im *)data;

	int clientsock = acceptsock(im->servfd);
	if (clientsock < 0)
	{
		debuginfo("%s->%s failed clientsock=%d, errno=%d", "acceptconn", "accept", clientsock, errno);
		return NULL;
	}

	debuginfo("accpet client success, cliid=%d", clientsock);

	//将客户端socket设置为non_blocked
	setnoblock(clientsock);

	//将客户端套接字注册事件
	struct event *uevent = setevent(im->reactor, clientsock, EV_READ, readwrite, NULL);
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
			if (errno == EINTR)
			{
//				freeevent(uevent);
			}

			debuginfo("%s->%s failed sockid=%d errno=%d", "readwrite", "recv", uevent->fd, errno);
			
			return NULL;
		}

		uevent->readbufsize = recvlen;
		uevent->readbuf[recvlen] = '\0';

//		debuginfo("%s->%s sockid=%d, add=%p, data=", "readwrite", "recv", uevent->fd, uevent);//, uevent->readbuf
		rdata *data = new handlereq();
		data->handleevent(uevent->readbuf, uevent->readbufsize, 1);
		delete data;

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
			debuginfo("%s->%s failed clientsock=%d", "readwrite", "addevent", urevent->fd);
			return NULL;
		}
	}
	else if (uevent->eventtype & EV_WRITE)
	{
		/*发送信息*/
		int sendlen = send(uevent->fd, uevent->writebuf, uevent->writebufsize, 0);
		if (sendlen <= 0)
		{
			if (errno == EINTR)
			{
//				freeevent(uevent);
			}

			debuginfo("%s->%s failed sockid=%d errno=%d", "readwrite", "send", uevent->fd, errno);

			return NULL;
		}

//		debuginfo("%s->%s sockid=%d, add=%p, data=", "readwrite", "send", uevent->fd, uevent);//, uevent->writebuf

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

	return (void *)SUCCESS_STR;
}

