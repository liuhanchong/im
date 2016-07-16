#!/usr/bin/python
#test_pconfig.py

import sys
sys.path.append("..")
import pyutil.pconfig as pcon

#test main fun
if __name__ == '__main__' :
	print 'test parse config fun-----------------------------'

	f = pcon.openconfig('server.ini')
	if f == None :
		print 'open file faield'
		sys.exit()

	print pcon.pastring(f, 'MYSQLDB', 'DB', "123")
	print pcon.pastring(f, 'MYSQLDB', 'Passwd', "234")
	print pcon.pastring(f, 'MYSQLDB', 'mmmm', "mmm")
	print pcon.paint(f, 'CONNPOOLNUMBER', 'CoreConnNumber', 20)
	print pcon.paint(f, 'ACCSOCKETOVERTIME', 'AccOutTimeThreadLoopSpace', 20)
	print pcon.paint(f, 'AIO', 'CoreConnNumber', 3320)
	print pcon.paint(f, 'SERVER', 'AccThreadLoopSpace', 20)
	print pcon.pafloat(f, 'AIO', 'AccThreadLoopSpace', 20.3)
	print pcon.pafloat(f, 'AIO', 'MaxBufferLength', 20.3)

	pcon.closeconfig(f)