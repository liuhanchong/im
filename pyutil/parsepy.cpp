#include "parsepy.h"

parsepy::parsepy()
{
	init();
}

parsepy::~parsepy()
{
	finalize();
}

int parsepy::runpy(char *py)
{
	return PyRun_SimpleString(py);
}

pyobject *parsepy::getattr(pyobject *moudle, char *attr)
{
	return PyObject_GetAttrString(moudle, attr);
}

pyobject *parsepy::newtuple(int size)
{
	return PyTuple_New(size);
}

void parsepy::intuple(pyobject *tuple, int seq, pyobject *object)
{
	PyTuple_SetItem(tuple, seq, object);
}

pyobject *parsepy::call(pyobject *fun, pyobject *arg)
{
	return PyObject_CallObject(fun, arg);
}

void parsepy::init()
{
	Py_Initialize();
}

void parsepy::finalize()
{
	Py_Finalize();
}

pyobject *parsepy::import(char *model)
{
	return PyImport_ImportModule(model);
}

pyobject *parsepy::pysfs(char *str)
{
	return PyString_FromString(str);
}

pyobject *parsepy::parseobj(pyobject *ret)
{
	pyobject *obj = NULL;
	if (PyArg_Parse(ret, "O", &obj) == 0)
	{
		return NULL;
	}

	return obj;
}

char *parsepy::parsestr(pyobject *ret)
{
	char *str = NULL;
	if (PyArg_Parse(ret, "s", &str) == 0)
	{
		return NULL;
	}

	return str;
}

int parsepy::parseint(pyobject *ret)
{
	int va = 0;
	if (PyArg_Parse(ret, "i", &va) == 0)
	{
		return 0;
	}

	return va;
}

void parsepy::decref(pyobject *obj)
{
	Py_DECREF(obj);
}

void parsepy::decrefex(pyobject **obja, int size)
{
	for (int i = 0; i < size; i++)
	{
		Py_DECREF(obja[i]);
	}	
}

pyobject *parsepy::exem(char *mo, char *attr, pyobject *arg)
{
	pyobject *moudle = import(mo);
	if (!moudle)
	{
		return NULL;
	}

	pyobject *fun = getattr(moudle, attr);
	if (!fun)
	{
		decref(moudle);
		return NULL;
	}

	pyobject *ret = call(fun, arg);

	decref(moudle);
	decref(fun);

	return ret;
}

pyobject *parsepy::setargtuple(pyobject **arg, int size)
{
	pyobject *argtuple = newtuple(size);
	if (argtuple)
	{
		for (int i = 0; i < size; i++)
		{
			intuple(argtuple, i, arg[i]);	
		}
	}
	return argtuple;
}

pyobject *parsepy::getdict(pyobject *moudle)
{
	return PyModule_GetDict(moudle);
}

pyobject *parsepy::getdictitem(pyobject *dict, char *item)
{
	return PyDict_GetItemString(dict, item);
}

pyobject *parsepy::insclass(char *mo, char *cla, pyobject *arg, pyobject *kw)
{
	pyobject *moudle = import(mo);
	if (!moudle)
	{
		return NULL;
	}

	pyobject *dict = getdict(moudle);
	if (!dict)
	{
		decref(moudle);
		return NULL;
	}

	pyobject *claobj = getdictitem(dict, cla);
	if (!claobj)
	{
		decref(dict);
		decref(moudle);
		return NULL;
	}

	return PyInstance_New(claobj, arg, kw);
}

pyobject *parsepy::callmethod(pyobject *cla, char *method)
{
	return PyObject_CallMethod(cla, method, NULL);
}