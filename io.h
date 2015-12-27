#ifndef IO_H
#define IO_H

#include <fcntl.h>

/*打开文件*/
int openfile(const char *path, int flag, mode_t mode);

/*复制一个文件描述符，共享一个文件句柄*/
int dupfile(const int fileno);

/*删除文件*/
int rmfile(const char *path);

/*关闭描述符*/
int closefile(int fileno);

/*读取一行*/
int readline(int fileno, char *text, int size);

/*读数据*/
int readfile(int fileno, char *text, int size);

/*从头跳转指定位置*/
int offsethead(int fileno, off_t off);

/*从当前位置跳转*/
int offsetcur(int fileno, off_t off);

/*获取文件大小,计算单位为（M）*/
int filelen(int fileno);

/*写入文件*/
int writefile(int fileno, const char *text, int size);

#endif
