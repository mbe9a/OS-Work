#include "shell.h"

#define MAX_LENGTH	100

int length;

char validchars[66] = {'A','a','B','b','C','c','D','E','e','F','f','G','g','H','h','I','i',
'J','j','K','k','L','l','M','m','N','n','O','o','P','p','Q','q','R','r','S','s','T','t','U',
'u','V','v','W','w','X','x','Y','y','Z','z','0','1','2','3','4','5','6','7','8','9','-','_','.','/'};

bool word(char* str) {					// check if char* is only made of validchars
	bool test = false;
	for (unsigned int i = 0; i < strlen(str); i++) {
		for(int i = 0; i < 66; i++) {
			test = false;
			if(*str == validchars[i]) {
				test = true;
				break;
			}
		}
		str++;
	}
	return test;
}

int lt_gt(char* str) {					// determine file redirections within token
	bool lt = false;					// for parsing < and > without spaces
	bool gt = false;
	for(char* iter = str; *iter; ++iter) {
		if (*iter == '>') {
			gt = true;
		} else if (*iter == '<') {
			lt = true;
		}
	}
	if(lt && gt) {
		return 3;
	} else if (gt) {
		return 2;
	} else if (lt) {
		return 1;
	} else {
		return 0;
	}
}

void printNestedVector(vector<vector<char*> > vec) {
	vector<vector<char*> >::iterator it1;
	vector<char*>::iterator it2;
	for(it1 = vec.begin(); it1 < vec.end(); it1++) {
		vector<char*> temp = *it1;
		for(it2 = temp.begin(); it2 < temp.end(); it2++) {
			printf("\n%s", *it2);
		}
	}
}

void printVector(vector<char*> vec) {
	vector<char*>::iterator iter;
	for (iter = vec.begin(); iter < vec.end(); iter++) {
		printf("\n%s", *iter);
	}
}

void PrintError() {
	printf("ERROR: invalid word.\n");
}

void parse(vector<char*> list) {
	// this functions as a statemachine for parsing text and executing token groups (no piping)
	// function is called within recursive piping
	vector<char*>::iterator iter = list.begin();
	ParseState state = Start;
	char* command;
	vector<char*> args;
	int inFile = 0;
	int outFile = 0;
	// int pid;
	// int fd[2];
	while(!list.empty()) {
		if(iter == list.end()) {						// go to execute state, break while loop
				list.clear();
				state = ExecuteCommand;
		}
		switch(state) {
			case Start:								// get command
				if(!word(*iter)) {
					PrintError();
					list.clear();
					break;
				} else {
					command = *iter;
					args.push_back(*iter);
					iter++;
					state = CheckWord;
				}
				break;
			case CheckWord:							// if word, add to args
				if(!word(*iter)) {
					state = CheckOperator;
				} else {
					args.push_back(*iter);
					iter++;
				}
				break;
			case CheckOperator:						// if operator, handle files, otw -> ERROR
				if(strcmp(*iter, "<") == 0) {
					iter++;
					state = InputRedirection;
				} else if(strcmp(*iter, ">") == 0) {
					iter++;
					state = OutputRedirection;
				} else {
					printf("test");
					list.clear();
				}
				break;
			case InputRedirection:					// handle input file pipe
				inFile = open(*iter, O_RDONLY);
				iter++;
				state = CheckWord;
				break;
			case OutputRedirection:					// handle output file pipe
				outFile = open(*iter, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				iter++;
				state = CheckWord;
				break;
			case ExecuteCommand:					// set up and call execvp by forking a child process
				int fd[2];
				pipe(fd);
				int pid_u = fork();
				if(pid_u == 0) {					// fork process
					if(inFile != 0) {				// set up input pipe
						dup2(inFile, 0);
						close(inFile);
					} else {
				 		close(fd[1]);
					} if (outFile != 0) {			// set up output pipe
						dup2(outFile, 1);
						close(outFile);
					}
					char* c_args[args.size() + 1];
					vector<char*>::iterator iter2;
					int i = 0;
					for(iter2 = args.begin(); iter2 < args.end(); iter2++) {
						c_args[i] = *iter2;
						i++;
					}
					c_args[i] = NULL;
					if(execvp(command, c_args) < 0) {
						printf("ERROR: exec failed.\n");
						exit(1);
					}
				} else {
					close(fd[0]);
					close(fd[1]);
					wait(NULL);
				}
				break;
		}
	}
}

void close_pipes(int pipes[]) {
	for (int i = 0; i < length; i++) {
		close(pipes[i]);
	}
}

void pipe_recursion(vector<vector<char*> > vec, int pipes[], int size, int input, int output) {
	vector<char*> current = vec.back();
	vec.pop_back();
	if (vec.size() == 0) {								// base case, only pipe input
		if(fork() == 0) {
			dup2(pipes[length - 2], 0);
			close_pipes(pipes);
			parse(current);
			exit(0);
		} else {
			return;
		}
	} else if (vec.size() == unsigned(size)) {						// initial case, only pipe output
		if(fork() == 0) {
			dup2(pipes[1], 1);
			close_pipes(pipes);
			parse(current);
			exit(0);
		}else {
			input += 2;
			output += 2;
			pipe_recursion(vec, pipes, size, input, output);
		}
	} else {											// else pipe based on position in groups
		if(fork() == 0) {
			dup2(pipes[input], 0);
			dup2(pipes[output], 1);
			close_pipes(pipes);
			parse(current);
			exit(0);
		} else {
			input += 2;
			output += 2;
			pipe_recursion(vec, pipes, size, input, output);
		}
	}
}

int main(int argc, char** argv) {
	char *buf = (char*)malloc(sizeof(char)*1024);
	char *token;
	vector<char*> tokens;
	vector< vector<char*> > groups;
	while(1) {
		printf("> ");
		fgets(buf, 1024, stdin);										// fill char buffer with console input
		int x;
		for (x = 0; (unsigned)x < strlen(buf); x++) {					// remove trailing newline or carriage return
			if ( buf[x] == '\n' || buf[x] == '\r' ) {
                buf[x] = '\0';
			}
		}
		if (strlen(buf) > MAX_LENGTH) {									// throw error if over 80 chars
			printf("\nERROR: maximum number of characters (%i) exceeded.\n", MAX_LENGTH);
		}
		else if (strcmp(buf, "exit") == 0) {							// exit program if input is 'exit'
			break;
		}
		else {
			token = strtok(buf," ");									// split buffer based on spaces and add to a list
			while (token != NULL) {
				tokens.push_back(token);
				token = strtok(NULL," ");
			}
			vector<char*>::iterator it;
			vector<char*> temp;
			for (it = tokens.begin(); it < tokens.end(); it++) {		// split each token based on pipes and put into nested list
				if (strcmp(*it, "|") == 0) {
					groups.push_back(temp);
					temp.clear();
					continue;
				}
				else if (it == tokens.end() - 1) {
					temp.push_back(*it);
					groups.push_back(temp);
					temp.clear();
					continue;
				}
				temp.push_back(*it);
			}
			tokens.clear();
			vector<vector<char*> >::iterator its;
			vector<vector<char*> > groups_real;
			for(its = groups.begin(); its < groups.end(); its++) {		// this for loop was added to handle file redirectors
				vector<char*> print = *its;								// not separated by spaces
				for (it = print.begin(); it < print.end(); it++) {
					if(!word(*it)) {
						int check = lt_gt(*it);
						char * dummy = *it;
						if(strcmp(*it, "<") == 0) {
							tokens.push_back((char*)"<");
						} else if (strcmp(*it, ">") == 0) {
							tokens.push_back((char*)">");
						} else if (check == 3) { 						// both
							char * lt = strtok(dummy, "<");
							tokens.push_back(lt);
							tokens.push_back((char*)"<");
							lt = strtok(NULL, "<");
							tokens.push_back(lt);
							char * gt = strtok(lt, ">");
							tokens.push_back((char*)">");
							gt = strtok(NULL, ">");
							tokens.push_back(gt);
						} else if (check == 2) { 						// gt
							char * gt = strtok(dummy, ">");
							tokens.push_back(gt);
							tokens.push_back((char*)">");
							gt = strtok(NULL, ">");
							tokens.push_back(gt);
						} else if (check == 1) { 						// lt
							char * lt = strtok(dummy, "<");
							tokens.push_back(lt);
							tokens.push_back((char*)"<");
							lt = strtok(NULL, "<");
							tokens.push_back(lt);
						} else {
							PrintError();
						}
					} else {
						tokens.push_back(*it);
					}
				}
				groups_real.push_back(tokens);
				tokens.clear();
			}
			vector<vector<char*> > reverse;
			vector<vector<char*> >::iterator r_it;
			for(r_it = groups_real.end() - 1; r_it >= groups_real.begin(); r_it--) {		// reverse the groups for recursive piping
				reverse.push_back(*r_it);
			}
			groups_real.swap(reverse);
			int size = groups_real.size();
			if(groups_real.size() > 1){													// if there are pipes, use recursive piping function
				length = (groups_real.size() - 1) * 2;
				int pipes[length];
				for (int i = 0; i < length / 2; i++) {
					pipe(pipes + 2*i);
				}
				pipe_recursion(groups_real, pipes, groups_real.size(), -2, 1);
				close_pipes(pipes);
				for (int i = 0; i < size; i++) {
					wait(NULL);
				}
			} else {																	// else just parse the one group
				parse(groups_real.front());
			}
			temp.clear();																// clear all the lists
			tokens.clear();
			groups.clear();
			groups_real.clear();
		}
	}
	return 0;
}
