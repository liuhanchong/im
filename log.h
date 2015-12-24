#ifndef LOG_H
#define LOG_H

/*标记errno号码的类型*/
typedef int errno_t;

/*初始化log*/
int openlog();

/*释放log*/
int closelog();

/*打印程序的调试信息*/
void debuginfo(const char *format, ...);

/*打印执行错误信息*/
void errorinfo(const char *format, ...);

/*打印系统错误信息*/
void errorinfo_errno(const char *fun, errno_t errorno);

/*调用此函数后程序自动退出*/
void dumpinfo(const char *format, ...);

#endif

