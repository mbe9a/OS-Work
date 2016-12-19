## Michael Eller mbe9a
## OS Machine Problem 3
## 22 November 2016
## makefile

OBJS = file_manager.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

fat: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o fat

file_manager.o: file_manager.h file_manager.cpp
	$(CC) $(CFLAGS) file_manager.cpp

clean:
	\rm *.o *~ fat

tar:
	tar cfv mbe9a.tar fat file_manager.h file_manager.cpp makefile mbe9a.pdf