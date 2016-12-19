#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/wait.h>
#include <string.h>
#include <cstring>
#include <vector>
using namespace std;

enum ParseState{Start, CheckWord, CheckOperator, InputRedirection, OutputRedirection, ExecuteCommand};
struct executable{char * command; vector<char*> args;};
bool word(char* str);
int lt_gt(char* str);
void parse(vector<char*> list);
void printNestedVector(vector<vector<char*> > vec);
void printVector(vector<char*> vec);
void pipe_recursion(vector<vector<char*> > vec, int pipes[], int size, int input, int output);
void close_pipes(int pipes[]);

#endif