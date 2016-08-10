bcore = ../core
blog = ${bcore}/log
bsock = ${bcore}/sock
blfile = ./logfile
bmain = .
bconf = ./conf
f = -DPRINTDEBUG
CFLAGS = ${f}

run :
	make clear link
	${bmain}/test

link : ${bmain}/test_minheap.c util io log minheap
	gcc -o test ${bmain}/test_minheap.c util.o io.o log.o minheap.o 

log : ${blog}/log.* ${bcore}/util.h ${blog}/io.h
	gcc ${CFLAGS} -c ${blog}/log.* ${bcore}/util.h ${blog}/io.h

io : ${blog}/io.* 
	gcc -c ${blog}/io.*

util : ${bcore}/util.* 
	gcc -c ${bcore}/util.*

minheap : ${bcore}/minheap.* ${bcore}/util.h
	gcc -c ${bcore}/minheap.* ${bcore}/util.h

clear :
	rm -f *.o ${bcore}/*.gch ${blog}/*.gch ${bsock}/*.gch ${bmain}/test ${blfile}/*.log
	rm -fd ${blfile}
	clear
