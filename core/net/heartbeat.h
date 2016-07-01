#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "../thread.h"
#include "reactor.h"

/*保存的信息种类*/
#define INFOTYPENUM 3

typedef struct heartbeat
{
	int (*fd)[INFOTYPENUM];/*保存fd信息 fd*[0]=fd fd*[1]=failednum fd*[2]=state:0-无效 1-有效*/
	time_t *fdtime;/*保存每个fd最后接收数据时间*/
	int connnum;/*保存连接数*/
	thread *hbthread;/*监听fd并标记失败数*/
	int connouttime;/*连接超时时间 单位:s*/
	int maxouttcount;/*最多超时次数*/
	struct reactor *reactor;/*保存其对应的反应堆*/
} heartbeat;

heartbeat *createheartbeat(int connnum, int connouttime);/*创建心跳监听*/
int delheartbeat(heartbeat *hebeat, int fd);/*删除一个心跳*/
int addheartbeat(heartbeat *hebeat, int fd);/*添加一个心跳*/
int upheartbeat(heartbeat *hebeat, int fd);/*更新心跳*/
int destroyheartbeat(heartbeat *hebeat);/*释放心跳监听*/

#endif