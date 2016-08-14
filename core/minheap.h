#ifndef MINHEAP_H
#define MINHEAP_H

typedef struct heapnode
{
	void *data;
	int pos;/*从1开始*/
} heapnode;

typedef struct minheap
{
	struct heapnode **head;
	int size;/*保存整个数组大小*/
	int cursize;/*当前保存的节点数*/
	int addsize;/*每次自动增加的节点数*/
} minheap;

struct minheap *createminheap(int size);
int addhn(struct minheap *heap, void *data);
int delhn(struct minheap *heap, void *data);
void *getminvalue(struct minheap *heap);
int getheapsize(struct minheap *heap);
int heapempty(struct minheap *heap);
int reverseminheap(struct minheap *heap, int size);
void *getvaluebyindex(struct minheap *heap, int index);
int destroyminheap(struct minheap *heap);

#endif