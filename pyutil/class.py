#!/usr/bin/python
#class.py

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
