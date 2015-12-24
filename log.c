#include "log.h"
#include "common.h"
#include "io.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

#define LOGTYPE 3
#define LOGSIZE 400
#define LOGNAMELEN 256

/*日志模块的配置信息结构体*/
typedef struct log
{
	/*运行状态*/
	int run;
	/*保存日志文件类型的文件描述符*/
	int filearray[LOGTYPE]; /*0-debug 1-error 2.dump*/
	/*日志锁*/
	pthread_mutex_t logmutex;
} log;

static log logs; 

static int combinelog(char *title, char *log, va_list arg_list, const char *format)
{
	/*定义打印调试信息*/
	char info[LOGSIZE];
	if (vsnprintf(info, LOGSIZE, format, arg_list) > 0)
	{
		return snprintf(log, LOGSIZE + 100, "%s:%s\r\n", title, info);
	}

	return 0;
}

static void generatefilename(char *name, int size, char *title)
{
	memset(name, 0, size);

	const int timesize = 20;
	char timestr[timesize];
	time_t curtime = time(NULL);
	struct tm *strutm = gmtime(&curtime);

	if (strutm && 
		snprintf(timestr, timesize, "%d%d%d%d%d%d", 
			strutm->tm_year, strutm->tm_mon, strutm->tm_mday,
			 strutm->tm_hour, strutm->tm_min, strutm->tm_sec) > 0)
	{
		snprintf(name, size, "%s_%s.log", title, timestr);
		return;
	}

	strncpy(name, "temp.log", 8);
}

static void writelog(int logtype, const char *log, int size)
{
	int fileno = -1;
	switch (logtype)
	{
	case 1: /*debug*/
		fileno = logs.filearray[0];
		break;

	case 2: /*error*/
		fileno = logs.filearray[1];
		break;

	case 3: /*dump*/
		fileno = logs.filearray[2];
		break;

	default:
		fileno = logs.filearray[0];
		break;
	}

	if (writefile(fileno, log, size) != size)
	{
		printf("%s:%s,errorno:%d\n", "writelog", "日志写入失败", errno);
	}
}

/*初始化log*/
int openlog()
{
	memset(&logs, 0, sizeof(log));

	char *title[LOGTYPE] = 
	{
		"debug",/*debug信息*/
		"error",/*错误信息*/
		"dump"/*崩溃信息*/
	};

	int flag = O_WRONLY | O_CREAT | O_EXCL | O_APPEND;
	mode_t mode = S_IRUSR | S_IWUSR;
	char name[LOGNAMELEN];
	for (int i = 0; i < LOGTYPE; i++)
	{
		generatefilename(name, LOGNAMELEN, title[i]);
		logs.filearray[i] = openfile(name, flag, mode);
		if (logs.filearray[i] == -1)
		{
			return FAILED;
		}
	}

	pthread_mutexattr_t mutexattr;
	if (pthread_mutexattr_init(&mutexattr) != 0)
	{
		return FAILED;
	}

	if (pthread_mutex_init(&logs.logmutex, &mutexattr) != 0)
	{
		pthread_mutexattr_destroy(&mutexattr);
		return FAILED;
	}

	pthread_mutexattr_destroy(&mutexattr);
	
	logs.run = 1;

	return SUCCESS;
}

/*释放log*/
int closelog()
{
	logs.run = 0;

	for (int i = 0; i < LOGTYPE; i++)
	{
		closefile(logs.filearray[i]);
	}

	pthread_mutex_destroy(&logs.logmutex);

	return SUCCESS;
}

/*打印程序的调试信息*/
void debuginfo(const char *format, ...)
{

#ifndef PRINTDEBUG
	return;
#endif

	if (!format)
	{
		return;
	}

	int size = 0;
	char log[LOGSIZE + 100];

	va_list arg_list;
	va_start(arg_list, format);
	size = combinelog("debug", log, arg_list, format);
	va_end(arg_list);

	if (size <= 0)
	{
		return;
	}

	writelog(1, log, size);
}

/*打印执行错误信息*/
void errorinfo(const char *format, ...)
{
	if (!format)
	{
		return;
	}

	int size = 0;
	char log[LOGSIZE + 100];

	va_list arg_list;
	va_start(arg_list, format);
	size = combinelog("error", log, arg_list, format);
	va_end(arg_list);

	if (size <= 0)
	{
		return;
	}

	writelog(2, log, size);
}

/*打印系统错误信息*/
void errorinfo_errno(const char *fun, errno_t errorno)
{
	if (!fun)
	{
		return;
	}

	/*定义打印调试信息*/
	char *perror = strerror(errorno);
	if (!perror)
	{
		return;
	}

	int size = 0;
	char log[LOGSIZE];
	size = snprintf(log, LOGSIZE, "error_errno:%s\r\n", perror);

	if (size <= 0)
	{
		return;
	}

	writelog(2, log, size);
}

/*调用此函数后程序自动退出*/
void dumpinfo(const char *format, ...)
{
	if (!format)
	{
		return;
	}

	int size = 0;
	char log[LOGSIZE + 100];

	va_list arg_list;
	va_start(arg_list, format);
	size = combinelog("dump", log, arg_list, format);
	va_end(arg_list);

	if (size > 0)
	{
		writelog(3, log, size);
	}

	exit(1);
}