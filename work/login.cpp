#include "login.h"
extern "C"
{
	#include "../core/util.h"
}

int login::handleevent(unsigned char *data, int size, int type)
{
	debuginfo("login::handleevent, type=%d, data=%s, size=%d", type, data, size);

	return SUCCESS;
}