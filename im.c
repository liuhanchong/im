#include "common.h"
#include "io.h"
#include "thread.h"
#include "queue.h"
#include "reactor.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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

void *callback_t(void *arg)
{
	debuginfo("callback_t-----%d", *((int *)arg));

	return NULL;
}

int main()
{
	//2.共享队列互斥处理
	openlog();

	reactor *reactor = NULL;
	if ((reactor = createreactor()) != NULL)
	{
		int i = 100;
		event *uevent = setevent(reactor, 1, callback_t, &i);
		if (addevent(uevent, EV_READ | EV_PERSIST) == SUCCESS)
		{
			printf("%s\n", "addevent ok");
		}

		uevent = setevent(reactor, 2, callback_t, &i);
		if (addevent(uevent, EV_READ | EV_PERSIST) == SUCCESS)
		{
			printf("%s\n", "addevent ok");
		}

		struct timespec timer = {2, 0};
		uevent = settimer(reactor, callback_t, &i, &timer);
		if (addtimer(uevent, 0) == SUCCESS)
		{
			printf("%s\n", "addtimer ok");
		}

		int j = 101;
		struct timespec timer2 = {3, 0};
		uevent = settimer(reactor, callback_t, &j, &timer2);
		if (addtimer(uevent, 0) == SUCCESS)
		{
			printf("%s\n", "addtimer ok");
		}

		if (dispatchevent(reactor) == SUCCESS)
		{
			printf("%s\n", "dispatchevent ok");
		}

		if (destroyreactor(reactor) == SUCCESS)
		{
			debuginfo("%s\n", "reactor ok");
		}
	}

	closelog();

	// openlog();

	// struct list list;
	// createqueue(&list, 100, 2, quesort_t);
	// if (empty(&list))
	// {
	// 	printf("%s\n", "empty");

	// 	int array[10] = {0,9,3,7,6,4,1,2,8,0};
	// 	for (int i = 0; i < 4; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

	// 	for (int i = 0; i < 6; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

	// 	for (int i = 6; i < 10; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

	// 	for (int i = 0; i < 6; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	for (int i = 0; i < 6; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	for (int i = 0; i < 6; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

	// 	queuenode head;
	// 	for (int i = 0; i < 32; i++)
	// 	{
	// 		if (pop(&list, &head))
	// 		{
	// 			printf("%s %d\n", "the first is", *((int *)head.data));
	// 		}
	// 	}

	// 	queuenode *node = NULL;
	// 	if ((node = gethead(&list)))
	// 	{
	// 		printf("%s %d\n", "the first is head", *((int *)node->data));	
	// 	}

	// 	if (destroyqueue(&list))
	// 	{
	// 		printf("%s\n", "destroyqueue success");
	// 	}
	// }

	// closelog();

	// openlog();

	// struct list list;
	// createqueue(&list, 100, 1, NULL);
	// if (empty(&list))
	// {
	// 	printf("%s\n", "empty");

	// 	int array[10] = {5,9,8,7,6,0,1,2,3,4};
	// 	for (int i = 0; i < 3; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

	// 	queuenode head;
	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	for (int i = 3; i < 6; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	for (int i = 6; i < 10; i++)
	// 	{
	// 		push(&list, &array[i], array[i]);
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	queuenode *node = NULL;
	// 	if ((node = gethead(&list)))
	// 	{
	// 		printf("%s %d\n", "the first is head", *((int *)node->data));	
	// 	}

	// 	if (destroyqueue(&list))
	// 	{
	// 		printf("%s\n", "destroyqueue success");
	// 	}
	// }

	// closelog();

	// struct list list;
	// createqueue(&list, 10, 0, NULL);
	// if (empty(&list))
	// {
	// 	printf("%s\n", "empty");

	// 	int i = 2;
	// 	int j = 1;
	// 	push(&list, &j, 0);
	// 	push(&list, &i, 0);
	// 	push(&list, &i, 0);

	// 	printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

	// 	push(&list, &i, 0);
	// 	push(&list, &i, 0);
	// 	push(&list, &i, 0);

	// 	push(&list, &i, 0);
	// 	push(&list, &i, 0);
	// 	push(&list, &i, 0);

	// 	push(&list, &i, 0);
	// 	push(&list, &i, 0);
	// 	if (!push(&list, &i, 0) && !push(&list, &i, 0) && !push(&list, &i, 0))
	// 	{
	// 		printf("%s\n", "push error");
	// 	}

	// 	printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

	// 	setmaxqueuelen(&list, 100);

	// 	printf("%ld %ld %d\n", getcurqueuelen(&list), getmaxqueuelen(&list), full(&list));

	// 	queuenode head;
	// 	if (pop(&list, &head))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)head.data));
	// 	}

	// 	queuenode *node = NULL;
	// 	if ((node = gethead(&list)))
	// 	{
	// 		printf("%s %d\n", "the first is", *((int *)node->data));	
	// 	}

	// 	if (destroyqueue(&list))
	// 	{
	// 		printf("%s\n", "destroyqueue success");
	// 	}
	// }
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