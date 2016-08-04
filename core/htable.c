#include "htable.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

static unsigned long strhash(hkey str) 
{ 
	if (str == NULL)
	{
		return 0; 
	}

	int i = 0, l = 0; 
	unsigned long ret = 0; 
	unsigned short *s = NULL;

	l = (strlen(str) + 1) / 2; 
	s = (unsigned short *)str; 
	for (i = 0; i < l; i++)
	{
		ret ^= (s[i]<<(i & 0x0f)); 
	}
	return ret; 
}

static void freekey(htnode *htnode)
{
	free(htnode->key);
}

static void freeitem(htnode *htnode)
{
	free(htnode->item);
}

static int equalkey(hkey skey, hkey dkey)
{
	return (strcmp(skey, dkey) == 0) ? 1 : 0;
}

hashtable *createhashtable(int tlen)
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

	newhtable->tmslot = NULL;

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

	return newhtable;
}

int destroyhashtable(hashtable *htable)
{
	if (!htable)
	{
		return SUCCESS;
	}

	//释放表结点
	struct htnode *htnode = NULL;
	struct htnode *htnextnode = NULL;
	for (int i = 0; i < htable->tablelen; i++)
	{
		htnode = htable->hashtable[i];
		while (htnode)
		{
			htnextnode = htnode->next;
			freekey(htnode);
			free(htnode);
			htnode = htnextnode;
		}
	}
	free(htable->hashtable);

	pthread_mutex_destroy(&htable->tablemutex);

	free(htable);

	return SUCCESS;
}

int setitem(hashtable *htable, hkey key, htitem item)
{
	htnode *newhtnode = (struct htnode *)malloc(sizeof(struct htnode));
	if (!newhtnode)
	{
		return FAILED;
	}

	/*复制key值*/
	newhtnode->ksize = strlen(key);
	newhtnode->key = malloc(newhtnode->ksize + 1);
	memcpy(newhtnode->key, key, newhtnode->ksize);
	newhtnode->key[newhtnode->ksize] = '\0';

	/*value 直接使用用户的空间*/
//	newhtnode->isize = isize;
//	newhtnode->item = malloc(newhtnode->isize);
//	memcpy(newhtnode->item, item, newhtnode->isize);
	newhtnode->item = item;

	newhtnode->hkid = 0;
	newhtnode->next = NULL;

	int index = strhash(key) % htable->tablelen;
	struct htnode *fnode = htable->hashtable[index];
	if (!fnode)
	{
		htable->hashtable[index] = newhtnode;
	}
	else
	{
		while (fnode->next)
		{
			fnode = fnode->next;
		}
		fnode->next = newhtnode;
	}

	return SUCCESS;
}

int delitem(hashtable *htable, hkey key)
{
	int index = strhash(key);
	index = index % htable->tablelen;

	int ret = FAILED;
	htnode *prehtnode = NULL;
	struct htnode *fnode = htable->hashtable[index];
	while (fnode)
	{
		if (equalkey(fnode->key, key) == 1)
		{
			if (!prehtnode)
			{
				htable->hashtable[index] = fnode->next;
			}
			else
			{
				prehtnode->next = fnode->next;
			}

			freekey(fnode);
			free(fnode);

			ret = SUCCESS;
			break;
		}

		prehtnode = fnode; 
		fnode = fnode->next;
	}

	return ret;
}

htitem getitemvalue(hashtable *htable, hkey key)
{
	int index = strhash(key);
	index = index % htable->tablelen;

	htitem item = NULL;
	struct htnode *fnode = htable->hashtable[index];
	while (fnode)
	{
		if (equalkey(fnode->key, key) == 1)
		{
			item = fnode->item;
			break;
		}
		fnode = fnode->next;
	}

	return item;
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