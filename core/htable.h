#ifndef HTABLE_H
#define HTABLE_H

#include <pthread.h>

#define htitem void*

typedef int (*htset)(void *htable, htitem arg);
typedef htitem (*htget)(void *htable, void *arg);
typedef int (*htdel)(void *htable, void *arg);

typedef struct htnode
{
	htitem item;
	struct htnode *next;
} htnode;

typedef struct hashtable
{
	htnode **hashtable;/*hash列表*/
	pthread_mutex_t tablemutex;/*互斥锁*/
	int tablelen;/*散列表长度*/
	htset set;/*设置方式*/
	htget get;/*获取方式*/
	htdel del;/*删除方式*/
} hashtable; 

hashtable *createhashtable(int tlen, htset set, htget get, htdel del);
int destroyhashtable(hashtable *htable);
int setitem(hashtable *htable, htitem item);
htitem getitemvalue(hashtable *htable, void *key);
int delitem(hashtable *htable, htitem item);
int delitemex(hashtable *htable, void *key);
int excap(hashtable *htable, int tlen);/*hashtable扩容*/

#endif