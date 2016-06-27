#include "./core/util.h"
#include "./core/log/log.h"
#include "./core/thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

int main()
{
	if (openlog())
	{
		char str[100];
		int size = readline(0, str, 100);
		printf("%s\n", strerror(errno));
		printf("%d_%s_%d\n", size, str, filelen(0));

		time_t t1 = time(NULL);
		for (int i = 0; i <= 10000000; i++)
		{
			debuginfo("%s_%s_test", "hello", "liuhanchong");
			errorinfo("%s_%s_test", "hello", "liuhanchong");
			errorinfo_errno("main", 100);			
		}
		time_t t2 = time(NULL);
		printf("__________________%ld\n", (long)(t2 -t1));


		debuginfo("%s_%s_test", "hello", "liuhanchong");
		errorinfo("%s_%s_test", "hello", "liuhanchong");
		errorinfo_errno("main", 100);
//		dumpinfo("%s_%s_test", "hello", "liuhanchong");

		debuginfo("%s_%s_test", "hello", "liuhanchong");
		errorinfo("%s_%s_test", "hello", "liuhanchong");
		errorinfo_errno("main", 100);

		debuginfo("%s_%s_test", "hello", "liuhanchong");
		errorinfo("%s_%s_test", "hello", "liuhanchong");
		errorinfo_errno("main", 100);

		debuginfo("%s_%s_test", "hello", "liuhanchong");
		errorinfo("%s_%s_test", "hello", "liuhanchong");
		errorinfo_errno("main", 100);
//		dumpinfo("%s_%s_test", "hello", "liuhanchong");

		if (closelog())
		{
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