#include "rdata.h"
extern "C"
{
	#include "../core/util.h"
}

rdata::rdata()
{
}

rdata::~rdata()
{
}

int rdata::parse(unsigned char *data, int size)
{
	return SUCCESS;
}

int rdata::parsehead(unsigned char *data, int size)
{
	return SUCCESS;
}

int rdata::parsesize(unsigned char *data, int size)
{
	return SUCCESS;
}

int rdata::parsetype(unsigned char *data, int size)
{
	return SUCCESS;
}

int rdata::parsedata(unsigned char *data, int size)
{
	return SUCCESS;
}
