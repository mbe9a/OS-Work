# Michael Eller mbe9a
# Machine Problem 2
# 29 September 2016
# makefile

OBJS = maximum_finder.o barrier.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

max: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -lpthread -o max

shell.o: maximum_finder.h maximum_finder.cpp
	$(CC) $(CFLAGS) maximum_finder.cpp

barrier.o: barrier.h barrier.cpp
	$(CC) $(CFLAGS) barrier.cpp

clean:
	\rm *.o *~ max

tar:
	tar cfv mbe9a.tar max maximum_finder.h maximum_finder.cpp barrier.h barrier.cpp makefile mbe9a.pdf
