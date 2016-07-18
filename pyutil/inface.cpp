#include "inface.h"
#include "parsepy.h"

int test(char *rstr, int size)
{
	parsepy py;

	py.runpy((char *)"import sys");
	py.runpy((char *)"sys.path.append('..')");

	pyobject *obj[1] = {py.pysfs((char *)"server.ini")};
	pyobject *argtuple = py.setargtuple(obj, 1);
	if (!argtuple)
	{
		return 1;
	}

	pyobject *ret = py.exem((char *)"pyutil.pconfig", (char *)"openconfig", argtuple);
	if (!ret)
	{
		return 2;
	}

	pyobject *fp = py.parseobj(ret);
	if (!fp)
	{
		return 3;
	}
//	py.decref(ret);
//	py.decrefex(obj, 1);
//	py.decref(argtuple);

	pyobject *obj4[4] = {fp, 
						 py.pysfs((char *)"NET"),
						 py.pysfs((char *)"ip"),
						 py.pysfs((char *)"127.0.0.1")};
	argtuple = py.setargtuple(obj4, 4);
	if (!argtuple)
	{
		return 4;
	}
	ret = py.exem((char *)"pyutil.pconfig", (char *)"pastring", argtuple);
	if (!ret)
	{
		return 5;
	}

	char *str = py.parsestr(ret);
	if (!str)
	{
		return 6;
	}

	strncpy(rstr, str, size);

//	py.decref(ret);
//	py.decref(argtuple);

	argtuple = py.newtuple(1);
	py.intuple(argtuple, 0, fp);
	ret = py.exem((char *)"pyutil.pconfig", (char *)"closeconfig", argtuple);
	if (!ret)
	{
		return 7;
	}

	py.decref(ret);
//	py.decrefex(obj4, 4);
	py.decref(argtuple);

	return 0;
}