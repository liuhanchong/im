#include "../core/util.h"
#include "../core/minheap.h"
#include <stdlib.h>
#include <stdio.h>

// static inline int comparehenode(struct heapnode *srchenode, struct heapnode *deshenode)
// {
// 	int *i1 = srchenode->data;
// 	int *i2 = deshenode->data;
// 	return (*i1 < *i2) ? 1 : 
// 			(*i1 > *i2) ? -1 : 0; 
// }

void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void printheap(struct minheap *heap)
{
	for (int i = 0; i < heap->cursize; i++)
	{
		printf("%d ", *((int *)heap->head[i]->data));
	}
	printf("\n");
}

void printinfo(minheap *heap)
{
	if (heapempty(heap))
	{
		debuginfo("empty heap");
	}

	debuginfo("heap size %d", getheapsize(heap));
	printf("heap size %d, all size %d\n", getheapsize(heap), heap->size);
	printheap(heap);
}

int main(int argc, char *argv[])
{
	openlog();

	minheap *heap = createminheap(5);
	if (!heap)
	{
		debuginfo("createminheap failed");
		return 0;
	}

	const int size = 8;
	int numarray[size] = {49, 38, 65, 97, 76, 13, 27, 50};
	printinfo(heap);

	for (int i = 0; i < size / 2; i++)
	{
		addhn(heap, &numarray[i]);
		printinfo(heap);
	}

	for (int i = size / 2; i < size; i++)
	{
		addhn(heap, &numarray[i]);
		printinfo(heap);
	}

	int jj = 100;
	addhn(heap, &jj);
	printinfo(heap);

	int kk = 0;
	addhn(heap, &kk);
	printinfo(heap);

	int mm = 4;
	addhn(heap, &mm);
	printinfo(heap);

	int  nn = 11;
	addhn(heap, &nn);
	printinfo(heap);

	delhn(heap, &jj);
	printinfo(heap);

	delhn(heap, &numarray[0]);
	printinfo(heap);

	delhn(heap, &kk);
	printinfo(heap);

	if (destroyminheap(heap) == FAILED)
	{
		debuginfo("destroyminheap failed");
		return 0;
	}

	debuginfo("test success");

	closelog();

	return 0;
}