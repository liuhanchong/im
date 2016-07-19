#include "inface.h"
#include "parsepy.h"
#include "../core/util.h"
#include <exception> 
#include <string.h> 
using namespace std;

int test(char *rstr, int size)
{
	try
	{
		parsepy py;

		//import py moudle
		py.runpy((char *)"import sys");
		py.runpy((char *)"sys.path.append('..')");

		//open file
		pyobject *obj[1] = {py.pysfs((char *)"server.ini")};
		pyobject *argtuple = py.setargtuple(obj, 1);
		pyobject *ret = py.exem((char *)"pyutil.pconfig", (char *)"openconfig", argtuple);
		pyobject *fp = py.parseobj(ret);

		//get key value
		pyobject *obj4[4] = {fp, py.pysfs((char *)"NET"),
		 py.pysfs((char *)"ip"), py.pysfs((char *)"127.0.0.1")};
		argtuple = py.setargtuple(obj4, 4);
		ret = py.exem((char *)"pyutil.pconfig", (char *)"pastring", argtuple);
		strncpy(rstr, py.parsestr(ret), size);

		//close file
		argtuple = py.newtuple(1);
		py.intuple(argtuple, 0, fp);
		ret = py.exem((char *)"pyutil.pconfig", (char *)"closeconfig", argtuple);
	}
	catch (exception &e)
	{
		strncpy(rstr, e.what(), size);
		return FAILED;
	}

	return SUCCESS;
}

int getsyscon(const char *filename, sys *sysc)
{	
	try
	{
		parsepy py;

		//import py moudle
		py.runpy((char *)"import sys");
		py.runpy((char *)"sys.path.append('..')");

		pyobject *sysclass = py.insclass((char *)"pyutil.inface", (char *)"sys", NULL, NULL); 

		//read config
		pyobject *obj[2] = {py.pysfs((char *)filename), sysclass};
		pyobject *argtuple = py.setargtuple(obj, 2);
		pyobject *ret = py.exem((char *)"pyutil.inface", (char *)"getsyscon", argtuple);

		//获取类的成员
		pyobject *ip = py.callmethod(sysclass, (char *)"getip");
		char *cip = py.parsestr(ip);
		strncpy((char *)sysc->ip, cip, sizeof(sysc->ip));

		pyobject *port = py.callmethod(sysclass, (char *)"getport");
		sysc->port = py.parseint(port);
	}
	catch (exception &e)
	{
		return FAILED;
	}

	return SUCCESS;	
}