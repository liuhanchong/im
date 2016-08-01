#include "htable.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

hashtable *createhashtable(int tlen, htset set, htget get, htdel del)
{
	hashtable *newhtable = (struct hashtable *)malloc(sizeof(struct hashtable));
	if (!newhtable)
	{
		return NULL;
	}

	pthread_mutexattr_t mutexattr;
	if (pthread_mutexattr_init(&mutexattr) != 0 ||
		pthread_mutex_init(&newhtable->tablemutex, &mutexattr) != 0)
	{
		free(newhtable);
		return NULL;
	}

	tlen = (tlen > 0) ? tlen : 256;
	newhtable->tablelen = tlen;
	int tsize = sizeof(htnode *) * tlen;
	newhtable->hashtable = (struct htnode **)malloc(tsize);
	if (!newhtable->hashtable)
	{
		free(newhtable);
		return NULL;
	}
	memset(newhtable->hashtable, 0, tsize);

	newhtable->set = set;
	newhtable->get = get;
	newhtable->del = del;

	return newhtable;
}

int destroyhashtable(hashtable *htable)
{
	if (!htable)
	{
		return SUCCESS;
	}

	pthread_mutex_destroy(&htable->tablemutex);

	//释放表结点
	struct htnode *htnode = NULL;
	struct htnode *htnextnode = NULL;
	for (int i = 0; i < htable->tablelen; i++)
	{
		htnode = htable->hashtable[i];
		while (htnode)
		{
			htnextnode = htnode->next;
			free(htnode);
			htnode = htnextnode;
		}
	}
	free(htable->hashtable);

	free(htable);

	return SUCCESS;
}

int setitem(hashtable *htable, htitem item)
{
	int index = htable->set(htable, item);
	if (index < 0)
	{
		debuginfo("%s->%s find index failed", "setitem", "set");
		return FAILED;
	}

	htnode *newhtnode = (struct htnode *)malloc(sizeof(struct htnode));
	if (!newhtnode)
	{
		return FAILED;
	}
	newhtnode->item = item;
	newhtnode->next = NULL;

	index = index % htable->tablelen;
	struct htnode *fitem = htable->hashtable[index];
	if (!fitem)
	{
		htable->hashtable[index] = newhtnode;
	}
	else
	{
		while (fitem->next)
		{
			fitem = fitem->next;
		}
		fitem->next = newhtnode;
	}

	return SUCCESS;
}

htitem getitemvalue(hashtable *htable, void *key)
{
	return htable->get(htable, key);
}

int delitem(hashtable *htable, htitem item)
{
	int index = htable->set(htable, item);
	if (index < 0)
	{
		debuginfo("%s->%s find index failed", "delitem", "set");
		return FAILED;
	}

	index = index % htable->tablelen;
	struct htnode *fnode = htable->hashtable[index];
	if (!fnode)
	{
		return FAILED;
	}

	htnode *prehtnode = NULL;
	while (fnode)
	{
		if (fnode->item == item)
		{
			if (!prehtnode)
			{
				htable->hashtable[index] = fnode->next;
			}
			else
			{
				prehtnode->next = fnode->next;
			}

			free(fnode);
			break;
		}

		prehtnode = fnode; 
		fnode = fnode->next;
	}

	return SUCCESS;
}

int delitemex(hashtable *htable, void *key)
{
	return htable->del(htable, key);
}

int excap(hashtable *htable, int tlen)
{
	if (tlen <= htable->tablelen)
	{
		return SUCCESS;
	}

	int tsize = sizeof(htnode *) * tlen;
	htnode **echashtable = (struct htnode **)malloc(tsize);
	if (!echashtable)
	{
		return FAILED;
	}
	memset(echashtable, 0, tsize);

	struct htnode *htnode = NULL;
	for (int i = 0; i < htable->tablelen; i++)
	{
		htnode = htable->hashtable[i];
		if (htnode)
		{
			echashtable[i] = htnode;
		}
	}
	free(htable->hashtable);

	htable->hashtable = echashtable;
	htable->tablelen = tlen;

	return SUCCESS;
}