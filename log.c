#include "log.h"

/*打印程序的调试信息*/
void debuginfo(const char *format, ...)
{

#ifndef PRINTDEBUG
	return;
#endif

	if（!format）
	{
		return;
	}

	/*定义打印调试信息*/
	
}

/*打印执行错误信息*/
void errorinfo(const char *format, ...)
{

}

/*调用此函数后程序自动退出*/
void dumpinfo(const char *format, ...)
{

}