#ifndef INFACE_H
#define INFACE_H

typedef struct sys
{
	unsigned char ip[20];
	int port;

} sys;

#ifdef __cplusplus
extern "C"
{
#endif

int test(char *rstr, int size);

int getsyscon(const char *filename, sys *sysc);
 
#ifdef __cplusplus
}
#endif
 
#endif