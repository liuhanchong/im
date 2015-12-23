/*
日志信息
	*debug
	*程序执行
	*数据记录 
*/

#ifndef LOG_H
#define LOG_H

/*初始化log*/
int openlog();

/*释放log*/
int closelog();

/*打印程序的调试信息*/
void debuginfo(const char *format, ...);

/*打印执行错误信息*/
void errorinfo(const char *format, ...);

/*打印系统错误信息*/
void errorinfo_errno(const char *fun, errno_t errno);

/*调用此函数后程序自动退出*/
void dumpinfo(const char *format, ...);

#endif

