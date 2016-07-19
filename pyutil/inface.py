#!/usr/bin/python
#class.py

import pconfig as pcon

class sys :
	def __init__(self) :
		self.ip = ""
		self.port = 0

	def getip(self) :
		return self.ip

	def getport(self) :
		return self.port

	def setip(self, ip) :
		self.ip = ip

	def setport(self, port) :
		self.port = port

def getsyscon(filename, sysc) :
	f = pcon.openconfig(filename)
	if f == None :
		return 0

	ip = pcon.pastring(f, "NET", "ip", "127.0.0.1")
	sysc.setip(ip)

	port = pcon.paint(f, "NET", "port", 8888)
	sysc.setport(port)

	pcon.closeconfig(f)

	return 1

if __name__ == '__main__' :
	sysc = sys()
	ret = getsyscon('../conf/server.ini', sysc)
	if (ret == 1) :
		print sysc.getip(), sysc.getport()

