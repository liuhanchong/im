#include "timers.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

/*设置计时器事件*/
void settimer(event *event, reactor *reactor,
			 void *(*callback)(void *), void *arg, struct timespec *timer)
{
	event->fd = -1;
	event->reactor = reactor;
	event->callback = callback;
	event->arg = arg;
	memcpy(&event->timer, timer, sizeof(struct timespec));
	event->next = NULL;
}

/*添加计时器事件*/
int addtimer(event *event)
{
	event->eventflag = 0;

	//加入到用户事件列表
	struct event *eventtimer = (struct event *)event->reactor->timerslist;
	if (eventtimer == NULL)
	{
		event->reactor->timerslist = event;
	}
	else
	{
		while (eventtimer->next)
		{
			eventtimer = eventtimer->next;
		}
		eventtimer->next = event;
	}
	event->reactor->timersnumber++;
	
	return SUCCESS;
}

/*删除计时器事件*/
int deltimer(event *event)
{
	struct event *eventtimer = (struct event *)event->reactor->timerslist;

	struct event **headtimer = &eventtimer; 
	struct event *pretimer = eventtimer;
	while (eventtimer)
	{
		if (eventtimer->callback == event->callback 
			&& eventtimer->arg == event->arg
			&& eventtimer->timer.tv_sec == event->timer.tv_sec
			&& eventtimer->timer.tv_nsec == event->timer.tv_nsec)
		{
			if (eventtimer == *headtimer)
			{
				*headtimer = eventtimer->next;
			}
			else
			{
				pretimer->next = eventtimer->next;
			}
			free(eventtimer);

			event->reactor->timersnumber--;

			return SUCCESS;
		}

		pretimer = eventtimer;
		eventtimer = eventtimer->next;
	}

	return FAILED;
}

/*获取最小超时时间*/
int getminouttimers(reactor *reactor, struct timespec *mintimer)
{
	struct event *eventtimer = (struct event *)reactor->timerslist;
	if (eventtimer == NULL)
	{
		return FAILED;
	}

	memcpy(mintimer, eventtimer, sizeof(struct timespec));
	while (eventtimer->next)
	{
		eventtimer = eventtimer->next;
		if (eventtimer->timer.tv_sec < mintimer->tv_sec 
			|| (eventtimer->timer.tv_sec == mintimer->tv_sec 
				&& eventtimer->timer.tv_nsec < mintimer->tv_nsec))
		{
			memcpy(mintimer, eventtimer, sizeof(struct timespec));
		}
	}

	return SUCCESS;
}

/*遍历计时器事件*/
int looptimers(reactor *reactor)
{
	return SUCCESS;
}