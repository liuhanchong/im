#include "queue.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

static int deletenode(list *list, queuenode *node)
{
	if (node->next != NULL && node->pre != NULL)
	{
		node->next->pre = node->pre;
		node->pre->next = node->next;

		//删除第一个元素改变头指针
		if (node == list->head)
		{
			list->head = node->next;
		}

		//当链表为空将链表头置为空
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

int createqueue(list *list, list_t maxlen, int openprio, quesort sortfun)
{
	pthread_mutexattr_t mutexattr;
	if (pthread_mutexattr_init(&mutexattr) == 0 &&
		pthread_mutex_init(&list->thmutex, &mutexattr) == 0)
	{
		list->curqueuelen = 0;
		list->head = NULL;
		list->maxqueuelen = maxlen;
		list->openprio = openprio;
		list->sortfun = sortfun;
		return SUCCESS;
	}

	return FAILED;
}

int destroyqueue(list *list)
{
	if (clear(list) == FAILED)
	{
		return FAILED;
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

		list->curqueuelen++;

		return SUCCESS;
	}

	//找到头指针
	queuenode *head = list->head;

	//优先级队列
	if (list->openprio == 1 || list->openprio == 2)
	{
		while (head)
		{
			//找到合适的节点
			if ((list->openprio == 1 && node->prio < head->prio)
				|| (list->openprio == 2 && list->sortfun != NULL && list->sortfun(head, node) == -1))
			{
				node->next = head;
				node->pre = head->pre;
				head->pre->next = node;
				head->pre = node;

				//改变头结点
				if (head == list->head)
				{
					list->head = node;
				}

				list->curqueuelen++;

				return SUCCESS;
			}

			//遍历到最后一个元素
			if (head->next == list->head)
			{
				break;
			}

			head = head->next;
		}

		//没找到合适的节点
		node->pre = head;
		node->next = head->next;
		head->next->pre = node;
		head->next = node;

		list->curqueuelen++;

		return SUCCESS;
	}
	//非优先级队列
	else if (list->openprio == 0)
	{
		node->pre = head->pre;
		head->pre->next = node;
		head->pre = node;
		node->next = head;

		list->curqueuelen++;
	}

	return SUCCESS;
}

queuenode *gethead(list *list)
{
	return list->head;
}

int pop(list *list, queuenode *node)
{
	queuenode *head = list->head;
	if (head == NULL)
	{
		return FAILED;
	}
	memcpy(node, head, sizeof(queuenode));
	return deletenode(list, head);
}

int full(list *list)
{
	return (((list->curqueuelen < list->maxqueuelen) || list->maxqueuelen <= 0) ? 0 : 1);
}

int clear(list *list)
{
	while (!empty(list))
	{
		deletenode(list, gethead(list));
	}

	return SUCCESS;
}

int del(list *list, queuenode *node)
{
	return deletenode(list, node);
}

