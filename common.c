#include "common.h"
#include <sys/resource.h>

int getmaxfilenumber()
{
	struct rlimit rlt;
	if (getrlimit(RLIMIT_NOFILE, &rlt) == 0)
	{
		return rlt.rlim_cur;
	}

	return 1024;
}

int setmaxfilenumber(int filenumber)
{
	struct rlimit rlt;
	if (getrlimit(RLIMIT_NOFILE, &rlt) == 0)
	{
		rlt.rlim_cur = (filenumber >= rlt.rlim_max) ? rlt.rlim_max : filenumber;
		if (setrlimit(RLIMIT_NOFILE, &rlt) == 0)
		{
			return SUCCESS;
		}
	}

	return FAILED;
}

int setcorefile(int filesize)
{
	struct rlimit rlt;
	if (getrlimit(RLIMIT_CORE, &rlt) == 0)
	{
		rlt.rlim_cur = (filesize >= rlt.rlim_max) ? rlt.rlim_max : filesize;
		if (setrlimit(RLIMIT_CORE, &rlt) == 0)
		{
			return SUCCESS;
		}
	}

	return FAILED;
}

int getcorefilesize()
{
	struct rlimit rlt;
	if (getrlimit(RLIMIT_CORE, &rlt) == 0)
	{
		return rlt.rlim_cur;
	}

	return 0;
}