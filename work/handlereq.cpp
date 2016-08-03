#include "handlereq.h"
#include "login.h"
extern "C"
{
	#include "../core/util.h"
}
#include <stdlib.h>

int handlereq::handleevent(unsigned char *data, int size, int type)
{
	rdata *rdata = NULL;
	switch (type)
	{
		case 0:
			debuginfo("handle req");
			break;

		case 1: /*登录*/
			rdata = new login();
			break;

		case 2:
			break;

		case 3:
			break;

		default:
			break;
	}

	rdata->handleevent(data, size, type);

	delete rdata;

	return SUCCESS;
}