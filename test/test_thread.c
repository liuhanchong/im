#include "./core/util.h"
#include "./core/thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

void *fun1(void *data)
{
	printf("%s-1\n", (char *)data);

	for (int i = 0; i <= 20000; i++)
	{
		debuginfo("%s_%s_test", "hello", "liuhanchong");
	}

	return NULL;
}

void *fun2(void *data)
{
	printf("%s-2\n", (char *)data);

	return NULL;
}

int main()
{
	if (openlog())
	{
		printf("%s\n", "1");
		thread *thread1 = createthread(fun1, "wo shi fun1", 2);
		thread *thread2 = createthread(fun2, "wo shi fun2", -1);

		printf("%s\n", "2");
		if (!thread1 || !thread2)
		{
			printf("%s\n", "thread create failed");
		}

		for (int i = 0; i < 11; i++)
		{
			enablethread(thread2, 1);
			sleep(1);
		}

		printf("%s\n", "3");

		if (!destroythread(thread1) || !destroythread(thread2))
		{
			printf("%s\n", "destroythread failed");
		}

		printf("%s\n", "4");


		if (closelog())
		{
			printf("%s\n", "5");
			printf("%s\n", "init log!");
		}
		else
		{
			printf("2-%s\n", strerror(errno));
		}
	}
	else
	{
		printf("1-%s\n", strerror(errno));	
	}
	
	return 0;
}