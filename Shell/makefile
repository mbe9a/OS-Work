#Michael Eller
#mbe9a
#OS Fall 2016

OBJS = shell.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

msh : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o msh

shell.o : shell.h shell.cpp
	$(CC) $(CFLAGS) shell.cpp

clean:
	\rm *.o *~ msh

tar:
	tar cfv mbe9a.tar msh shell.cpp shell.h
