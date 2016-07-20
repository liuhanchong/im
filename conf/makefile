bcore = ../core
blog = ${bcore}/log
bsock = ${bcore}/net
bdb = ${bcore}/db
blfile = ./logfile
bmain = ..
bconf = ./conf
bpy = ../pyutil
f = -DPRINTDEBUG
CFLAGS = ${f}
cc = gcc

run :
	make clear 
	make link
	clear
	${bmain}/im &
	cd .
	top

link : ${bmain}/im.c util io log thread queue socket reactor io tpool hbeat parsepy inface
	$(cc) -lstdc++ -L/usr/lib/python2.7/ -lpython2.7 ${bmain}/im.c util.o io.o log.o thread.o queue.o socket.o reactor.o tpool.o hbeat.o parsepy.o inface.o -o ../im

log : ${blog}/log.* ${bcore}/util.h ${blog}/io.h
	$(cc) ${CFLAGS} -c ${blog}/log.* ${bcore}/util.h ${blog}/io.h

io : ${blog}/io.* 
	$(cc) -c ${blog}/io.*

util : ${bcore}/util.* 
	$(cc) -c ${bcore}/util.*

thread : ${bcore}/thread.* ${bcore}/util.h
	$(cc) -c ${bcore}/thread.* ${bcore}/util.h

queue : ${bcore}/queue.* ${bcore}/util.h 
	$(cc) -c ${bcore}/queue.* ${bcore}/util.h 

socket : ${bcore}/util.h ${bsock}/socket.* 
	$(cc) -c ${bcore}/util.h ${bsock}/socket.* 

reactor : ${bsock}/reactor.* ${bcore}/util.h ${bsock}/socket.h
	$(cc) -c ${bsock}/reactor.* ${bcore}/util.h ${bsock}/socket.h

tpool : ${bcore}/tpool.* ${bcore}/util.h ${bcore}/queue.h
	$(cc) -c ${bcore}/tpool.* ${bcore}/util.h ${bcore}/queue.h

hbeat : ${bcore}/util.h ${bsock}/hbeat.* ${bcore}/thread.h
	$(cc) -c ${bcore}/util.h ${bsock}/hbeat.* ${bcore}/thread.h

mydb : ${bcore}/util.h ${bdb}/mydb.*
	$(cc) -c ${bcore}/util.h ${bdb}/mydb.* 

parsepy : ${bpy}/parsepy.cpp ${bpy}/parsepy.h
	g++ -c ${bpy}/parsepy.cpp

inface : ${bpy}/inface.cpp ${bpy}/inface.h
	g++ -c ${bpy}/inface.cpp

clear :
	rm -f *.o ${bcore}/*.gch ${blog}/*.gch ${bsock}/*.gch ${bmain}/im ${blfile}/*.log ${bpy}/*.pyc ${bpy}/*.gch
	rm -fd ${blfile}
	rm -f pid
	clear

clearlog :
	rm -f ${blfile}/debug_* ${blfile}/dump_* ${blfile}/error_*

stop :
	${bmain}/im stop

stop_s : 
	kill -SIGINT ${pid}
	top
