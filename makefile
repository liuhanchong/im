run :
	./im

link : im.o io.o common.o log.o thread.o
	gcc -o im im.o io.o common.o log.o thread.o

im : im.c log.o 
	gcc -c im.c log.o

log : log.h log.c common.o io.o
	gcc -c log.h log.c common.o io.o

io : io.h io.c
	gcc -c io.h io.c

common : common.h 
	gcc -c common.h 

thread : thread.h thread.c common.h
	gcc -c thread.h thread.c common.h

clear :
	rm -f *.o *.gch im 

clearlog :
	rm -f ./logfile/debug_* ./logfile/dump_* ./logfile/error_*
