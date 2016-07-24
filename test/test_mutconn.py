#!/usr/bin/python
#sock.py

import socket
import thread
import threading

run = True
sarray = []
tarray = []

def proreq(seq, sarray, tarray) :
	try :
		s = socket.socket()
		s.connect(('192.168.10.123', 6666))
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
	for i in range(0, 100) :
		thread.start_new_thread(proreq, (i, sarray, tarray, ))
except :
	print "Error: unable to start thread"

try :
	while True :
		pass
except :
	run = False
	print '\nstop'




