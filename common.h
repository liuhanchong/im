#ifndef COMMON_H
#define COMMON_H

/*函数返回值*/
#define SUCCESS 1
#define FAILED 0

/*打印debug信息*/
#define PRINTDEBUG

#define lock(thmutex) (pthread_mutex_lock(&thmutex))

#define unlock(thmutex) (pthread_mutex_unlock(&thmutex))

#include "log.h"

int getmaxfilenumber();

int setmaxfilenumber(int filenumber);

int setcorefilesize(int filesize);

int getcorefilesize();

#endif