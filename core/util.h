#ifndef UTIL_H
#define UTIL_H

#include "./log/log.h"
#include <sys/time.h>

/*函数返回值*/
#define SUCCESS 1
#define FAILED 0
#define SUCCESS_STR "success"

#define lock(thmutex) (pthread_mutex_lock(&thmutex))
#define unlock(thmutex) (pthread_mutex_unlock(&thmutex))

int getmaxfilenumber();

int setmaxfilenumber(int filenumber);

int setcorefilesize(int filesize);

int getcorefilesize();

int getpidfromfile();

int setpidtofile();

int getcpucorenum();

int timespeccompare(struct timespec *src, struct timespec *dest);

#endif