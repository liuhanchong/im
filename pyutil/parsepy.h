#ifndef PARSEPY_H
#define PARSEPY_H

#include "../core/util.h"
#include <python2.7/Python.h>

typedef PyObject pyobject;

class parsepy 
{
public:
	parsepy();
	~parsepy();

public:
	int runpy(char *py);
	pyobject *import(char *model);
	pyobject *getattr(pyobject *moudle, char *attr);
	pyobject *newtuple(int size);
	void intuple(pyobject *tuple, int seq, pyobject *object);
	pyobject *call(pyobject *fun, pyobject *arg);
	void init();
	void finalize();
	pyobject *pysfs(char *str);
	pyobject *parseobj(pyobject *ret);
	char *parsestr(pyobject *ret);
	int parseint(pyobject *ret);
	void decref(pyobject *obj);
	void decrefex(pyobject **obja, int size);
	pyobject *exem(char *mo, char *attr, pyobject *arg);
	pyobject *setargtuple(pyobject **arg, int size);
	pyobject *getdict(pyobject *moudle);
	pyobject *getdictitem(pyobject *dict, char *item);
	pyobject *insclass(char *mo, char *cla, pyobject *arg, pyobject *kw);
	pyobject *callmethod(pyobject *cla, char *method);

private:

public:

private:

};

#endif