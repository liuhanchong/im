#ifndef UTIL_H
#define UTIL_H

#include "./log/log.h"

/*函数返回值*/
#define SUCCESS 1
#define FAILED 0

#define lock(thmutex) (pthread_mutex_lock(&thmutex))
#define unlock(thmutex) (pthread_mutex_unlock(&thmutex))

//#ifdef __cplusplus
//extern "C" {
//#endif

//#ifdef __cplusplus
//}
//#endif

int getmaxfilenumber();

int setmaxfilenumber(int filenumber);

int setcorefilesize(int filesize);

int getcorefilesize();

int getpidfromfile();

int setpidtofile();

#endif