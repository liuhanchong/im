#!/usr/bin/python
#sock.py

import socket
import thread
import threading
import sys
sys.path.append("..")
import pyutil.inface as inface

run = True
sarray = []
tarray = []

def proreq(seq, sarray, tarray, sysc) :
	try :
		s = socket.socket()

		s.connect((sysc.getip(), sysc.getport()))
		sarray.append(s) 
		tarray.append(threading.currentThread())

		while (run) :
			s.send('connect server')
			s.recv(1024)		
		s.close()
		print 'close client'
	except Exception:
		print 'send failed'
	else :
		s.close()
		print 'send success'

try :
	sysc = inface.sys()
	inface.getsyscon('../conf/server.ini', sysc)

	for i in range(0, 100) :
		thread.start_new_thread(proreq, (i, sarray, tarray, sysc, ))
except :
	print "Error: unable to start thread"

try :
	while True :
		pass
except :
	run = False
	print '\nstop'




