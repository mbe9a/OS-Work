// Michael Eller mbe9a
// OS Machine Problem 4
// 29 November 2016
// ftp.h (includes, defs, and protos)

#ifndef FTP_H
#define FTP_H

#define SRC125	"125 Data connection already open; transfer starting.\n"
#define SRC125LEN	53
#define SRC150	"150 File status okay; about to open data connection.\n"
#define SRC150LEN	53
#define SRC200	"200 The requested action has been successfully completed.\n"
#define SRC200LEN	58
#define SRC220	"220 Service ready for new user.\n"
#define SRC220LEN	32
#define SRC221	"221 Service closing control connection.\n"
#define SRC221LEN	40
#define SRC226	"226 Closing data connection. Requested file action successful (for example, file transfer or file abort).\n"
#define SRC226LEN	106
#define SRC451	"451 Requested action aborted. Local error in processing.\n"
#define SRC451LEN	57
#define SRC504	"504 Command not implemented for that parameter.\n"
#define SRC504LEN	48

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <list>
#include <errno.h>
#include <dirent.h>

using namespace std;

#endif