#ifndef TIMERS_H
#define TIMERS_H

#include "reactor.h"

/*设置计时器事件*/
void settimer(event *event, reactor *reactor,
				 void *(*callback)(void *), void *arg, struct timespec *timer);

/*添加计时器事件*/
int addtimer(event *event);

/*删除计时器事件*/
int deltimer(event *event);

/*获取最小超时时间*/
int getminouttimers(reactor *reactor, struct timespec *mintimer);

/*遍历计时器事件*/
int looptimers(reactor *reactor);

#endif 