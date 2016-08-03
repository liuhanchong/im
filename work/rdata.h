#ifndef RDATA_H
#define RDATA_H

class rdata
{
public:
	rdata();
	virtual ~rdata();

public:
	virtual int parse(unsigned char *data, int size);
	virtual int parsehead(unsigned char *data, int size);
	virtual int parsesize(unsigned char *data, int size);
	virtual int parsetype(unsigned char *data, int size);
	virtual int parsedata(unsigned char *data, int size);
	virtual int handleevent(unsigned char *data, int size, int type = 0) = 0;

private:

private:

};

#endif