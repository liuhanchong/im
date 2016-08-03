#ifndef HANDLEREQ_H
#define HANDLEREQ_H

#include "rdata.h"

class handlereq : public rdata
{
public:
	int handleevent(unsigned char *data, int size, int type = 0);

public:

private:

private:

};

#endif