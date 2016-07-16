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

run :
	make clear 
	make link
	${bmain}/im &
	cd .
	top

link : ${bmain}/im.c util io log thread queue socket reactor io tpool hbeat 
	gcc -L/usr/lib/python2.7/ -lpython2.7 -o ../im ${bmain}/im.c util.o io.o log.o thread.o queue.o socket.o reactor.o tpool.o hbeat.o 

log : ${blog}/log.* ${bcore}/util.h ${blog}/io.h
	gcc ${CFLAGS} -c ${blog}/log.* ${bcore}/util.h ${blog}/io.h

io : ${blog}/io.* 
	gcc -c ${blog}/io.*

util : ${bcore}/util.* 
	gcc -c ${bcore}/util.*

thread : ${bcore}/thread.* ${bcore}/util.h
	gcc -c ${bcore}/thread.* ${bcore}/util.h

queue : ${bcore}/queue.* ${bcore}/util.h 
	gcc -c ${bcore}/queue.* ${bcore}/util.h 

socket : ${bcore}/util.h ${bsock}/socket.* 
	gcc -c ${bcore}/util.h ${bsock}/socket.* 

reactor : ${bsock}/reactor.* ${bcore}/util.h ${bsock}/socket.h
	gcc -c ${bsock}/reactor.* ${bcore}/util.h ${bsock}/socket.h

tpool : ${bcore}/tpool.* ${bcore}/util.h ${bcore}/queue.h
	gcc -c ${bcore}/tpool.* ${bcore}/util.h ${bcore}/queue.h

hbeat : ${bcore}/util.h ${bsock}/hbeat.* ${bcore}/thread.h
	gcc -c ${bcore}/util.h ${bsock}/hbeat.* ${bcore}/thread.h

mydb : ${bcore}/util.h ${bdb}/mydb.*
	gcc -c ${bcore}/util.h ${bdb}/mydb.* 

clear :
	rm -f *.o ${bcore}/*.gch ${blog}/*.gch ${bsock}/*.gch ${bmain}/im ${blfile}/*.log ${bpy}/*.pyc
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
