#ifndef COMMON_H
#define COMMON_H

#include "log.h"

/*函数返回值*/
#define SUCCESS 1
#define FAILED 0

#define lock(thmutex) (pthread_mutex_lock(&thmutex))

#define unlock(thmutex) (pthread_mutex_unlock(&thmutex))

int getmaxfilenumber();

int setmaxfilenumber(int filenumber);

int setcorefilesize(int filesize);

int getcorefilesize();

#endif