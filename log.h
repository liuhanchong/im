/*
日志信息
	*debug
	*程序执行
	*数据记录 
*/

#ifndef LOG_H
#define LOG_H

/*打印程序的调试信息*/
void debuginfo(const char *format, ...);

/*打印执行错误信息*/
void errorinfo(const char *format, ...);

/*调用此函数后程序自动退出*/
void dumpinfo(const char *format, ...);

#endif

