#include "queue.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

static int deletenode(list *list, queuenode *node)
{
	if (node->next && node->pre)
	{
		node->next->pre = node->pre;
		node->pre->next = node->next;

		//删除第一个元素改变头指针
		if (node == list->head)
		{
			list->head = node->next;
		}

		if ((--list->curqueuelen) == 0)
		{
			list->head = NULL;
		}

		free(node);
		node = NULL;

		return SUCCESS;
	}

	return FAILED;
}

int createqueue(list *list, list_t maxlen, int openprio)
{
	if (!list || maxlen < 1)
	{
		return FAILED;
	}

	pthread_mutexattr_t mutexattr;
	if (pthread_mutexattr_init(&mutexattr) == 0 &&
		pthread_mutex_init(&list->thmutex, &mutexattr) == 0)
	{
		list->curqueuelen = 0;
		list->head = NULL;
		list->maxqueuelen = maxlen;
		list->openprio = openprio;
		return SUCCESS;
	}

	return FAILED;
}

int destroyqueue(list *list)
{
	if (!list)
	{
		return FAILED;
	}

	while (!empty(list))
	{
		deletenode(list, gethead(list));
	}

	if (pthread_mutex_destroy(&list->thmutex) != 0)
	{
		return FAILED;
	}

	return SUCCESS;
}

list_t getcurqueuelen(list *list)
{
	return list->curqueuelen;
}

list_t getmaxqueuelen(list *list)
{
	return list->maxqueuelen;
}

void setmaxqueuelen(list *list, list_t maxlen)
{
	list->maxqueuelen = maxlen;
}

int empty(list *list)
{
	return ((list->curqueuelen == 0) ? 1 : 0);
}

int push(list *list, void *data, int prio)
{
	if (full(list))
	{
		return FAILED;
	}

	queuenode *node = (queuenode *)malloc(sizeof(queuenode));
	if (!node)
	{
		return FAILED;
	}

	/*填充结构体*/
	node->data = data;
	node->next = NULL;
	node->pre = NULL;
	node->prio = prio;

	/*第一次插入元素*/
	if (empty(list))
	{
		list->head = node;
		node->next = node;
		node->pre = node;
	}
	/*链表存在元素*/
	else
	{
		//找到头指针
		queuenode *head = list->head;
		//优先级队列
		if (list->openprio == 1)
		{
			while (head->next != list->head)
			{
				if (node->prio < head->prio)
				{
					break;
				}

				head = head->next;
			}

			node->pre = head;
			node->next = head->next;
			head->next->pre = node;
			head->next = node;
		}
		//非优先级队列
		else
		{
			node->pre = head->pre;
			head->pre->next = node;
			head->pre = node;
			node->next = head;
		}
	}

	list->curqueuelen++;

	return 1;
}

queuenode *gethead(list *list)
{
	return list->head;
}

int pop(list *list, queuenode *node)
{
	queuenode *head = list->head;
	memcpy(node, head, sizeof(queuenode));
	return deletenode(list, head);
}

int full(list *list)
{
	return ((list->curqueuelen < list->maxqueuelen) ? 0 : 1);
}

