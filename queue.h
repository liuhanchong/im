#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

typedef unsigned long list_t;

typedef struct queuenode
{
	void *data;
	int prio;
	struct queuenode *next;
	struct queuenode *pre;
} queuenode;

/*循环链表*/
typedef struct list
{
	list_t curqueuelen;
	list_t maxqueuelen;
	queuenode *head;
	pthread_mutex_t thmutex;
	int openprio;/*开启队列优先级*/
} list;

int createqueue(list *list, list_t maxlen, int openprio);
int destroyqueue(list *list);
list_t getcurqueuelen(list *list);
void setmaxqueuelen(list *list, list_t maxlen);
list_t getmaxqueuelen(list *list);
int empty(list *list);
int push(list *list, void *data, int prio);
queuenode *gethead(list *list);
int pop(list *list, queuenode *node);
int full(list *list);

#endif 