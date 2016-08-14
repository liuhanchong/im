#include "rinface.h"
#include "../util.h"

int cre(int num)
{
	int fd = -1;
#if defined(_WIN32)
	#error no support operate system

#elif defined(__APPLE__) && defined(__MACH__)
	fd = kqueue();

#elif defined(__linux__) || defined(_AIX) || defined(__FreeBSD__) 
	fd = epoll_create(num);
			
#else
	#error no support operate system
#endif  

	return fd;
}

int ctlev(struct event *uevent)
{
	int filter = 0;
	int flags = 0;
	struct eventt addkevent;

#if defined(_WIN32)
	#error no support operate system

#elif defined(__APPLE__) && defined(__MACH__)
	filter = (uevent->eventtype & EV_READ) ? EVFILT_READ : EVFILT_WRITE;
	flags = (uevent->eventtype & EV_CTL_ADD) ? EV_ADD : EV_ADD;
	flags |= (uevent->eventtype & EV_PERSIST) ? 0 : EV_ONESHOT;
	evset(&addkevent, uevent->fd, filter, flags, 0, 0, NULL);
	if (kevent(uevent->reactor->reactorid, &addkevent, 1, NULL, 0, NULL) == -1)
	{
		return FAILED;
	}

#elif defined(__linux__) || defined(_AIX) || defined(__FreeBSD__) 
	filter = (uevent->eventtype & EV_READ) ? EPOLLIN : EPOLLOUT;
	filter |= (uevent->eventtype & EV_PERSIST) ? 0 : EPOLLONESHOT;
	flags = (uevent->eventtype & EV_CTL_MOD) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
	
	//加入到系统内核监听
	evset(&addkevent, uevent->fd, filter);
	if (epoll_ctl(uevent->reactor->reactorid, flags, uevent->fd, &addkevent) == -1)
	{
		return FAILED;
	}
			
#else
	#error no support operate system
#endif  

	return SUCCESS;
}

int waitev(struct reactor *reactor, struct timespec *ts)
{
	int actnum = -1;

#if defined(_WIN32)
	#error no support operate system

#elif defined(__APPLE__) && defined(__MACH__)
	actnum = kevent(reactor->reactorid, NULL, 0, reactor->kevelist, reactor->kevelistlen, ts);

#elif defined(__linux__) || defined(_AIX) || defined(__FreeBSD__) 
	int to = ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
	actnum = epoll_wait(reactor->reactorid, reactor->kevelist, reactor->kevelistlen, to);
			
#else
	#error no support operate system
#endif  

	return actnum;
}

int clo(int fd)
{
	return close(fd);
}

int getfd(struct eventt *uevent)
{
#if defined(_WIN32)
	#error no support operate system

#elif defined(__APPLE__) && defined(__MACH__)
	return uevent->ident;

#elif defined(__linux__) || defined(_AIX) || defined(__FreeBSD__) 
	return uevent->data.fd;
			
#else
	#error no support operate system
#endif 

	return -1;
}