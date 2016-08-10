#include "minheap.h"
#include "util.h"
#include "./net/rinface.h"

static inline int comparehenode(struct heapnode *srchenode, struct heapnode *deshenode)
{
	/*0-相等 1-源<目的 -1源>目的*/
	struct timespec *src = &((struct event *)srchenode->data)->endtimer;
	struct timespec *dest = &((struct event *)deshenode->data)->endtimer;

	return timespeccompare(src, dest);
}

static int moveup(minheap *heap, heapnode *node)
{
	if (node->pos == 1)
	{
		return SUCCESS;
	}

	heapnode *parnode = heap->head[(node->pos / 2) - 1];
	if (comparehenode(node, parnode) == 1)
	{
		void *data = node->data;
		node->data = parnode->data;
		parnode->data = data;

		return moveup(heap, parnode);
	}

	return SUCCESS;
}

static int movedown(minheap *heap, heapnode *node)
{
	if (node->pos > heap->cursize ||
		node->pos * 2 > heap->cursize || 
		node->pos * 2 + 1 > heap->cursize)
	{
		return SUCCESS;
	}

	heapnode *comnode = (comparehenode(heap->head[(node->pos * 2) - 1],
						 heap->head[node->pos * 2]) != -1) ?
	 			heap->head[(node->pos * 2) - 1] : heap->head[node->pos * 2];
	if (comparehenode(node, comnode) == -1)
	{
		void *data = node->data;
		node->data = comnode->data;
		comnode->data = data;

		return movedown(heap, comnode);
	}

	return SUCCESS;
}

struct minheap *createminheap(int size)
{
	struct minheap *heap = (struct minheap *)malloc(sizeof(struct minheap));
	if (!heap)
	{
		return NULL;
	}

	heap->head = (struct heapnode **)malloc(sizeof(struct heapnode *) * size);
	if (!heap->head)
	{
		free(heap);
		return NULL;
	}

	heap->size = size;
	heap->cursize = 0;

	return heap;
}

int addhn(struct minheap *heap, void *data)
{
	if (heap->size < heap->cursize + 1)
	{
		if (reverseminheap(heap, heap->size + 10) == FAILED)
		{
			return FAILED;
		}
	}

	struct heapnode *henode = (struct heapnode *)malloc(sizeof(struct heapnode));
	if (!henode)
	{
		return FAILED;
	}

	//将节点插入到完全二叉树末尾
	heap->head[heap->cursize++] = henode;
	henode->data = data;
	henode->pos = heap->cursize;

	/*将节点移动到合适的位置*/
	return moveup(heap, henode);
}

int delhn(struct minheap *heap, void *data)
{
	for (int i = 0; i < heap->cursize; i++)
	{
		if (heap->head[i]->data == data)
		{
			--heap->cursize;
			heap->head[i]->data = heap->head[heap->cursize]->data;
			int ret = movedown(heap, heap->head[i]);
			free(heap->head[heap->cursize]);
			return ret;
		}
	}

	return FAILED;
}

void *getminvalue(struct minheap *heap)
{
	return (heap->head) ? (heap->head[0]->data) : NULL;
}

unsigned int getheapsize(struct minheap *heap)
{
	return heap->cursize;
}

int heapempty(struct minheap *heap)
{
	return (heap->cursize == 0) ? 1 : 0;
}

int reverseminheap(struct minheap *heap, int size)
{
	if (size <= heap->size)
	{
		return FAILED;
	}

	struct heapnode **head = (struct heapnode **)malloc(sizeof(struct heapnode *) * size);
	if (!heap->head)
	{
		return FAILED;
	}

	for (int i = 0; i < heap->cursize; i++)
	{
		head[i] = heap->head[i];
	}
	free(heap->head);

	heap->head = head;
	heap->size = size;

	return SUCCESS;
}

int destroyminheap(struct minheap *heap)
{
	for (int i = 0; i < heap->cursize; i++)
	{
		free(heap->head[i]);
	}

	free(heap->head);
	free(heap);

	return SUCCESS;
}