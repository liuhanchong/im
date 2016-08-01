#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

typedef int list_t;

typedef struct queuenode
{
	void *data;
	int prio;
	struct queuenode *next;
	struct queuenode *pre;
} queuenode;

typedef int (*quesort)(queuenode *src, queuenode *dest);

/*循环链表*/
typedef struct list
{
	list_t curqueuelen;
	list_t maxqueuelen;
	queuenode *head;
	pthread_mutex_t thmutex;
	int openprio;/*开启队列优先级 0-不开启 1-开启 2-开启可排序队列*/
	quesort sortfun;/*开启排序队列后，传递的排序函数 0-相等 1-src<dest -1-src>desc*/
} list;

/*遍历所有的节点*/
#define looplist_for(list) \
	queuenode *headquenode = NULL; \
	queuenode *nextquenode = NULL; \
	for (headquenode = gethead((&list)), nextquenode = NULL;  \
		(headquenode != NULL && nextquenode != gethead((&list))); \
		 nextquenode = headquenode = headquenode->next) \

/*遍历所有的节点*/
#define looplist(list, headquenode) \
	(headquenode) = gethead(list); \
	while (headquenode) \
	{ 

#define endlooplist(list, headquenode) \
		if ((headquenode = (headquenode)->next) == (list)->head) \
		{ \
			break; \
		} \
	} 

int createqueue(list *list, list_t maxlen, int openprio, quesort sortfun);
int destroyqueue(list *list);
list_t getcurqueuelen(list *list);
void setmaxqueuelen(list *list, list_t maxlen);
list_t getmaxqueuelen(list *list);
int empty(list *list);
int push(list *list, void *data, int prio);
queuenode *gethead(list *list);
int pop(list *list, queuenode *node);
int full(list *list);
int clear(list *list);
int dele(list *list, queuenode *node);

#endif 