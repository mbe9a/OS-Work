## Michael Eller mbe9a
## OS Machine Problem 4
## 2 December 2016
## makefile

OBJS = ftp.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

my_ftpd: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o my_ftpd

ftp.o: ftp.h ftp.cpp
	$(CC) $(CFLAGS) ftp.cpp

clean:
	\rm *.o *~ my_ftpd

tar:
	tar cfv mbe9a.tar my_ftpd ftp.h ftp.cpp makefile mbe9a.pdf