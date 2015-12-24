#include "io.h"
#include <unistd.h>
#include <string.h>

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
	int sumlen = 0;
	const int tempsize = 50;
	char temptext[tempsize];
	while (sumlen <= size)
	{
		//读取固定长度的文本
		int len = read(fileno, temptext, tempsize);
		if (len == -1)
		{
			return -1;
		}
		else if (len == 0)
		{
			break;
		}

		//判断当前读取的字符串是否有'\n'
		int templen = sumlen;
		char *find = strchr(temptext, '\n'); 
		if (find)
		{
			*find = '\0';
			sumlen += strlen(temptext);
		}
		else
		{
			sumlen += tempsize;
		}

		strncpy((text + sumlen), temptext, tempsize - templen);
	}

	if (sumlen <= 0)
	{
		return 0;
	}

	return sumlen;
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
		off_t size = stats.st_size;
		return ((size / 1024) / 1024);
	}

	return -1;
}

/*写入文件*/
int writefile(int fileno, const char *text, int size)
{
	return write(fileno, text, size);
}