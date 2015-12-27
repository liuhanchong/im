#include "common.h"
#include "io.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>

#define LOGTYPE 3 /*日志的分类 debug error dump等*/
#define LOGSIZE 400 /*m每条日志信息最大长度*/
#define LOGNAMELEN 256 /*日志路径最大长度*/
#define LOGFILEMAXSIZE 5 /*每个日志文件最大的长度（MB）*/

/*日志模块的配置信息结构体*/
typedef struct log
{
	int run; /*运行状态*/
	int filearray[LOGTYPE]; /*保存日志文件类型的文件描述符 0-debug 1-error 2.dump*/
	pthread_mutex_t logmutex;/*日志锁*/
	char *title[LOGTYPE]; /*保存日志文件标题*/
} log;

static log logs; 

/*合并日志信息*/
static int combinelog(char *title, char *log, int logsize,
						 va_list arg_list, const char *format)
{
	char info[LOGSIZE];
	if (vsnprintf(info, LOGSIZE, format, arg_list) > 0)
	{
		return snprintf(log, logsize, "%s:%s\r\n", title, info);
	}

	return 0;
}

/*生成文件名*/
static int genfilename(char *name, int size, char *title)
{
	memset(name, 0, size);

	//获取当前时间
	time_t curtime = time(NULL);
	struct tm *strutm = localtime(&curtime);
	strutm->tm_year += 1900;
	strutm->tm_mon += 1;

	//组合文件名
	if (strutm && 
		snprintf(name, size, "%s_%d%d%d%d%d%d.log", title, 
			strutm->tm_year, strutm->tm_mon, strutm->tm_mday,
			 strutm->tm_hour, strutm->tm_min, strutm->tm_sec) > 0)
	{
		return SUCCESS;
	}

	strncpy(name, "temp.log", 8);
	return FAILED;
}

/*生成日志文件*/
static int genlogfile(char *title)
{
	char name[LOGNAMELEN];
	int flag = O_WRONLY | O_CREAT | O_EXCL | O_APPEND;
	mode_t mode = S_IRUSR | S_IWUSR;

	/*生成文件名*/
	if (genfilename(name, LOGNAMELEN, title) != SUCCESS)
	{
		return -1;
	}

	/*此处必须为创建新的不存在文件*/
	return openfile(name, flag, mode);
}

static int writelog(int logtype, const char *log, int size)
{
	int index = 0;
	switch (logtype)
	{
	case 1: /*debug*/
		index = 0;
		break;

	case 2: /*error*/
		index = 1;
		break;

	case 3: /*dump*/
		index = 2;
		break;

	default: /*debug*/
		index = 0;
		break;
	}

	int fileno = logs.filearray[index];
	char *title = logs.title[index];

	if (writefile(fileno, log, size) != size)
	{
		return FAILED;
	}

	//日志文件过大
	if (filelen(fileno) >= LOGFILEMAXSIZE)
	{
		int tempfileno = -1;
		if (((tempfileno = genlogfile(title)) == -1) || (closefile(fileno) != 0))
		{
			return FAILED;
		}

		//成功后才将最新的文件描述符保存
		logs.filearray[index] = tempfileno;
	}

	return SUCCESS;
}

/*初始化log*/
int openlog()
{
	memset(&logs, 0, sizeof(log));

	char *title[LOGTYPE] = 
	{
		"./logfile/debug",/*debug信息*/
		"./logfile/error",/*错误信息*/
		"./logfile/dump"/*崩溃信息*/
	};

	for (int i = 0; i < LOGTYPE; i++)
	{
		logs.title[i] = title[i];
	}

	for (int i = 0; i < LOGTYPE; i++)
	{
		/*此处必须为创建新的不存在文件*/
		if ((logs.filearray[i] = genlogfile(logs.title[i])) == -1)
		{
			return FAILED;
		}
	}

	/*初始互斥量*/
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
	
	/*日志模块运行*/
	logs.run = 1;

	return SUCCESS;
}

/*释放log*/
int closelog()
{
	logs.run = 0;

	for (int i = 0; i < LOGTYPE; i++)
	{
		if (closefile(logs.filearray[i]) != 0)
		{
			return FAILED;
		}
	}

	if (pthread_mutex_destroy(&logs.logmutex) != 0)
	{
		return FAILED;
	}

	return SUCCESS;
}

/*打印程序的调试信息*/
void debuginfo(const char *format, ...)
{
#ifndef PRINTDEBUG
	return;
#else
	if (!format)
	{
		return;
	}

	int size = 0;
	const int logsize = LOGSIZE + 20;
	char log[logsize];

	va_list arg_list;
	va_start(arg_list, format);
	size = combinelog("debug", log, logsize, arg_list, format);
	va_end(arg_list);

	if (size <= 0 || writelog(1, log, size) != SUCCESS)
	{
		//出现问题
	}
#endif

}

/*打印执行错误信息*/
void errorinfo(const char *format, ...)
{
	if (!format)
	{
		return;
	}

	int size = 0;
	const int logsize = LOGSIZE + 20;
	char log[logsize];

	va_list arg_list;
	va_start(arg_list, format);
	size = combinelog("error", log, logsize, arg_list, format);
	va_end(arg_list);

	if (size <= 0 || writelog(2, log, size) != SUCCESS)
	{
		//出现问题
	}
}

/*打印系统错误信息*/
void errorinfo_errno(const char *fun, errno_t errorno)
{
	if (!fun)
	{
		return;
	}

	char *perror = strerror(errorno);
	if (!perror)
	{
		return;
	}

	int size = 0;
	char log[LOGSIZE];
	size = snprintf(log, LOGSIZE, "error_errno:%s\r\n", perror);

	if (size <= 0 || writelog(2, log, size) != SUCCESS)
	{
		//出现问题
	}
}

/*调用此函数后程序自动退出*/
void dumpinfo(const char *format, ...)
{
	if (!format)
	{
		return;
	}

	int size = 0;
	const int logsize = LOGSIZE + 20;
	char log[logsize];

	va_list arg_list;
	va_start(arg_list, format);
	size = combinelog("dump", log, logsize, arg_list, format);
	va_end(arg_list);

	if (size <= 0 || writelog(3, log, size) != SUCCESS)
	{
		//出现问题
	}

	exit(1);
}