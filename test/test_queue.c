#include "./core/queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

int quesort_t(queuenode *src, queuenode *dest)
{
	int *srcuevent = (int *)src->data;
	int *destuevent = (int *)dest->data;

	if (*srcuevent > *destuevent)
	{
		return -1;
	}

	if (*srcuevent < *destuevent)
	{
		return 1;
	}	

	return 0;
}

int main()
{
	struct list list;
	createqueue(&list, 100, 0, NULL);
	if (empty(&list))
	{
		// printf("%s\n", "empty");

		printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

		looplist_test(list)
		{
			int *data = (int *)headquenode->data;
			printf("%d", *data);
		}
		printf("\n");

		int array[10] = {0,9,3,7,6,4,1,2,8,0};
		for (int i = 0; i < 1; i++)
		{
			push(&list, &array[i], array[i]);
		}

		printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

		looplist_test(list)
		{
			int *data = (int *)headquenode->data;
			printf("%d", *data);
		}
		printf("\n");

		for (int i = 1; i < 6; i++)
		{
			push(&list, &array[i], array[i]);
		}

		printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

		looplist_test(list)
		{
			int *data = (int *)headquenode->data;
			printf("%d ", *data);
		}
		printf("\n");

		for (int j = 1; j <= 19; j++)
		{
			for (int i = 1; i < 6; i++)
			{
				push(&list, &array[i], array[i]);
			}
		}

		printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

		looplist_test(list)
		{
			int *data = (int *)headquenode->data;
			printf("%d ", *data);
		}
		printf("\n");
	}

	return 1;
}