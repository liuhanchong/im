#include "io.h"
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define KB 1024
#define MB ((1024) * KB)

/*打开文件*/
int openfile(const char *path, int flag, mode_t mode)
{
	return open(path, flag, mode);
}

/*复制一个文件描述符，共享一个文件句柄*/
int dupfile(const int fileno)
{
	return dup(fileno);
}

/*删除文件*/
int rmfile(const char *path)
{
	return unlink(path);
}

/*关闭描述符*/
int closefile(int fileno)
{
	return close(fileno);
}

/*读取一行*/
int readline(int fileno, char *text, int size)
{
	int readsize = readfile(fileno, text, size);
	if (readsize <= 0)
	{
		return readsize;
	}

	char *find = strchr(text, '\n');
	if (find)
	{
		readsize = find - text;
		*find = '\0';

		//读取得数据多于一行 将其指针返回
		lseek(fileno, SEEK_CUR, (-(size - readsize - 1)));
	}

	return readsize;
}

/*读数据*/
int readfile(int fileno, char *text, int size)
{
	return read(fileno, text, size);
}

/*从头跳转指定位置*/
int offsethead(int fileno, off_t off)
{
	return lseek(fileno, off, SEEK_SET);
}

/*从当前位置跳转*/
int offsetcur(int fileno, off_t off)
{
	return lseek(fileno, off, SEEK_CUR);
}

/*获取文件大小,计算单位为（M）*/
int filelen(int fileno)
{
	struct stat stats;
	if (fstat(fileno, &stats) == 0)
	{
		return stats.st_size / MB;
	}
	return -1;
}

/*写入文件*/
int writefile(int fileno, const char *text, int size)
{
	return write(fileno, text, size);
}

/*获取文件选项*/
int getfcntl(int fileno)
{
	return fcntl(fileno, F_GETFL);
}