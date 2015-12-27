#include "common.h"
#include "io.h"
#include "thread.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main()
{
	struct list list;
	createqueue(&list, 10, 0);
	if (empty(&list))
	{
		printf("%s\n", "empty");

		int i = 2;
		int j = 1;
		push(&list, &j, 0);
		push(&list, &i, 0);
		push(&list, &i, 0);

		printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

		push(&list, &i, 0);
		push(&list, &i, 0);
		push(&list, &i, 0);

		push(&list, &i, 0);
		push(&list, &i, 0);
		push(&list, &i, 0);

		push(&list, &i, 0);
		push(&list, &i, 0);
		if (!push(&list, &i, 0) && !push(&list, &i, 0))
		{
			printf("%s\n", "push error");
		}

		printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

		setmaxqueuelen(&list, 100);

		printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

		queuenode head;
		if (pop(&list, &head))
		{
			printf("%s %d\n", "the first is", *((int *)head.data));
		}

		queuenode *node = NULL;
		if ((node = gethead(&list)))
		{
			printf("%s %d\n", "the first is", *((int *)node->data));	
		}

		if (destroyqueue(&list))
		{
			printf("%s\n", "destroyqueue success");
		}
	}
	return 0;
}

// void *fun1(void *data)
// {
// 	printf("%s-1\n", (char *)data);

// 	for (int i = 0; i <= 20000; i++)
// 	{
// 		debuginfo("%s_%s_test", "hello", "liuhanchong");
// 	}

// 	return NULL;
// }

// void *fun2(void *data)
// {
// 	printf("%s-2\n", (char *)data);

// 	return NULL;
// }

// int main()
// {
// 	if (openlog())
// 	{
// 		printf("%s\n", "1");
// 		thread *thread1 = createthread(fun1, "wo shi fun1", 2);
// 		thread *thread2 = createthread(fun2, "wo shi fun2", -1);

// 		printf("%s\n", "2");
// 		if (!thread1 || !thread2)
// 		{
// 			printf("%s\n", "thread create failed");
// 		}

// 		for (int i = 0; i < 11; i++)
// 		{
// 			enablethread(thread2, 1);
// 			sleep(1);
// 		}

// 		printf("%s\n", "3");

// 		if (!destroythread(thread1) || !destroythread(thread2))
// 		{
// 			printf("%s\n", "destroythread failed");
// 		}

// 		printf("%s\n", "4");


// 		if (closelog())
// 		{
// 			printf("%s\n", "5");
// 			printf("%s\n", "init log!");
// 		}
// 		else
// 		{
// 			printf("2-%s\n", strerror(errno));
// 		}
// 	}
// 	else
// 	{
// 		printf("1-%s\n", strerror(errno));	
// 	}
	
// 	return 0;
// }

// int main()
// {
// 	if (openlog())
// 	{
// 		char str[100];
// 		int size = readline(0, str, 100);
// 		printf("%s\n", strerror(errno));
// 		printf("%d_%s_%d\n", size, str, filelen(0));

// 		time_t t1 = time(NULL);
// 		for (int i = 0; i <= 10000000; i++)
// 		{
// 			debuginfo("%s_%s_test", "hello", "liuhanchong");
// 			errorinfo("%s_%s_test", "hello", "liuhanchong");
// 			errorinfo_errno("main", 100);			
// 		}
// 		time_t t2 = time(NULL);
// 		printf("__________________%ld\n", (long)(t2 -t1));


// 		debuginfo("%s_%s_test", "hello", "liuhanchong");
// 		errorinfo("%s_%s_test", "hello", "liuhanchong");
// 		errorinfo_errno("main", 100);
// //		dumpinfo("%s_%s_test", "hello", "liuhanchong");

// 		debuginfo("%s_%s_test", "hello", "liuhanchong");
// 		errorinfo("%s_%s_test", "hello", "liuhanchong");
// 		errorinfo_errno("main", 100);

// 		debuginfo("%s_%s_test", "hello", "liuhanchong");
// 		errorinfo("%s_%s_test", "hello", "liuhanchong");
// 		errorinfo_errno("main", 100);

// 		debuginfo("%s_%s_test", "hello", "liuhanchong");
// 		errorinfo("%s_%s_test", "hello", "liuhanchong");
// 		errorinfo_errno("main", 100);
// //		dumpinfo("%s_%s_test", "hello", "liuhanchong");

// 		if (closelog())
// 		{
// 			printf("%s\n", "init log!");
// 		}
// 		else
// 		{
// 			printf("2-%s\n", strerror(errno));
// 		}
// 	}
// 	else
// 	{
// 		printf("1-%s\n", strerror(errno));	
// 	}
	
// 	return 0;
// }