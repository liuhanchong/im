#include "./core/util.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include "./core/tpool.h"

void *tfun2(void *data)
{
	debuginfo("tfun2");
	sleep(3);
	
	return NULL;
}

void *tfun(void *data)
{
	tpool *ttpool = (tpool *)data;
	int count = 100;
	while (count--)
	{
		if (addttask(ttpool, tfun2, NULL))
		{
			debuginfo("%s %d", "tfun addtask success id = ", count);
		}
		else
		{
			debuginfo("%s %d", "tfun addtask failed id = ", count);	
		}
		sleep(1);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	openlog();
	
	tpool *ttpool = NULL;
	if ((ttpool = createtpool(10, 3)))
	{
		debuginfo("%s", "create t pool success");

		if (addthread(ttpool, 5))
		{
			debuginfo("%s%d", "add thread success tnum1 = ", 5);
		}
		else
		{
			debuginfo("%s%d", "add thread failed tnum1 = ", 5);	
		}

		if (addttask(ttpool, tfun, ttpool))
		{
			debuginfo("%s", "main addtask success");
		}

		if (addthread(ttpool, 5))
		{
			debuginfo("%s%d", "add thread success tnum2 = ", 5);
		}
		else
		{
			debuginfo("%s%d", "add thread failed tnum2 = ", 5);	
		}

		if (addthread(ttpool, 8))
		{
			debuginfo("%s%d", "add thread success tnum3 = ", 8);
		}
		else
		{
			debuginfo("%s%d", "add thread failed tnum3 = ", 8);	
		}

		if (addthread(ttpool, 5))
		{
			debuginfo("%s%d", "add thread success tnum4 = ", 5);
		}
		else
		{
			debuginfo("%s%d", "add thread failed tnum4 = ", 5);	
		}

		if (addthread(ttpool, 1))
		{
			debuginfo("%s%d", "add thread success tnum5 = ", 1);
		}
		else
		{
			debuginfo("%s%d", "add thread failed tnum5 = ", 1);	
		}

		sleep(70);

		if (destroytpool(ttpool))
		{
			debuginfo("%s", "destroy t pool success");
		}
	}

	closelog();

	return 0;
}