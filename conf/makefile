bcore = ../core
blog = ${bcore}/log
bsock = ${bcore}/net
bdb = ${bcore}/db
blfile = ./logfile
bmain = ..
bconf = ./conf
bpy = ../pyutil
btest = ../test
bwork = ../work
f = -DPRINTDEBUG
CFLAGS = ${f}
cc = gcc

run :
	make clear 
	make link
	clear
	${bmain}/im &
	cd .
	cd .
#	top

link : ${bmain}/im.c util io log thread queue htable db mydb \
	 socket minheap rinface reactor io tpool hbeat parsepy inface \
	 dbpool login rdata handlereq datainface 
	$(cc) -pthread util.o io.o log.o thread.o queue.o htable.o db.o mydb.o \
	socket.o minheap.o rinface.o reactor.o tpool.o hbeat.o parsepy.o inface.o \
	dbpool.o handlereq.o rdata.o login.o datainface.o ${bmain}/im.c \
	-o ../im -std=c99 -lstdc++ -I/usr/include/python2.7/ -L/usr/lib/python2.7/ -lpython2.7 -lmysqlclient

log : ${blog}/log.* ${bcore}/util.h ${blog}/io.h
	$(cc) ${CFLAGS} -std=c99 -c ${blog}/log.* ${bcore}/util.h ${blog}/io.h 
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

reactor : ${bsock}/reactor.* ${bcore}/util.h ${bsock}/socket.h ${bcore}/htable.h
	$(cc) -c ${bsock}/reactor.* ${bcore}/util.h ${bsock}/socket.h ${bcore}/htable.h

tpool : ${bcore}/tpool.* ${bcore}/util.h ${bcore}/queue.h
	$(cc) -std=c99 -c ${bcore}/tpool.* ${bcore}/util.h ${bcore}/queue.h

hbeat : ${bcore}/util.h ${bsock}/hbeat.* ${bcore}/thread.h
	$(cc) -std=c99 -c ${bcore}/util.h ${bsock}/hbeat.* ${bcore}/thread.h

htable : ${bcore}/util.h ${bcore}/htable.*
	$(cc) -std=c99 -c ${bcore}/util.h ${bcore}/htable.* 

parsepy : ${bpy}/parsepy.cpp ${bpy}/parsepy.h
	g++ -c ${bpy}/parsepy.cpp

inface : ${bpy}/inface.cpp ${bpy}/inface.h
	g++ -c ${bpy}/inface.cpp

mydb : ${bdb}/mydb.cpp ${bdb}/mydb.h
	g++ -c ${bdb}/mydb.cpp

db : ${bdb}/db.cpp ${bdb}/db.h
	g++ -c ${bdb}/db.cpp

dbpool : ${bdb}/dbpool.cpp ${bdb}/dbpool.h
	g++ -c ${bdb}/dbpool.cpp

login : ${bwork}/login.*
	g++ -c ${bwork}/login.cpp

rdata : ${bwork}/rdata.*
	g++ -c ${bwork}/rdata.cpp

handlereq : ${bwork}/handlereq.*
	g++ -c ${bwork}/handlereq.cpp

datainface : ${bwork}/datainface.*
	g++ -c ${bwork}/datainface.cpp

rinface : ${bsock}/rinface.* ${bcore}/util.h ${bcore}/queue.h ${bcore}/htable.h
	$(cc) -c ${bsock}/rinface.* ${bcore}/util.h ${bcore}/queue.h ${bcore}/htable.h

minheap : ${bcore}/util.h ${bcore}/minheap.*
	$(cc) -std=c99 -c ${bcore}/util.h ${bcore}/minheap.* 

clear :
	rm -f *.o ${bcore}/*.gch ${blog}/*.gch ${bsock}/*.gch ${blfile}/*.log ${bpy}/*.pyc ${bpy}/*.gch ${bdb}/*.gch
	rm -fd ${blfile}
	rm -f pid
	rm -f ${bmain}/im ${bmain}/tdb
	clear

clearlog :
	rm -f ${blfile}/debug_* ${blfile}/dump_* ${blfile}/error_*

stop :
	${bmain}/im stop

stop_s : 
	kill -SIGINT ${pid}
	top

row :
	make -s rownum

rownum :
	echo 'python代码行数 \c'
	find ${bmain} -name '*.py' | xargs grep -v "'^$'" | wc -l 
	echo 'c代码行数 \c'
	find ${bmain} -name "*.c" -or -name "*.h" | xargs grep -v "'^$'" | wc -l
	echo 'c++代码行数 \c'
	find ${bmain} -name "*.cpp" | xargs grep -v "'^$'" | wc -l
	echo 'total代码行数 \c'
	find ${bmain} -name '*.py' -or -name "*.c" -or -name "*.h" -or -name "*.cpp" | xargs grep -v "'^$'" | wc -l 

tlink : util io log queue ${btest}/${tn} ${bdb}/mydb.* ${bdb}/db.* ${bdb}/dbpool.* 
	g++ -c ${bdb}/mydb.cpp
	g++ -c ${bdb}/db.cpp
	g++ -c ${bdb}/dbpool.cpp
	g++ io.o util.o log.o queue.o db.o mydb.o dbpool.o ${btest}/${tn} -o ../tdb -lmysqlclient

trun :
	make clear
	make tlink tn=test_db.cpp
	../tdb &



