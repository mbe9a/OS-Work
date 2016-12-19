#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <deque>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <fcntl.h>

#define MAX_LINE_LENGTH 100

using namespace std;

void shrinkWhitespaces(string &str);
deque<string> tokenizeString(string &str, string &delim);
void trim(string &str);
void readNonEmptyLine(string &line);
bool validLine(string line);
bool validCommand(string command, int index, int commandCount);

int main(int argc, char *argv[]) {

	string line;
	string argDelim = string(" ");
	string pipeDelim = string("|");

	while(true) {
		
		// read input
		readNonEmptyLine(line);
		if (strcmp(line.c_str(), "exit") == 0) break;

		// check if the line is valid or not
		if (!validLine(line)) {
			printf("invalid input\n");
			fflush(stdout);
			continue;
		}

		// separate the commands from the line
		deque<string> commandList1 = tokenizeString(line, pipeDelim);
		int commandCount = commandList1.size();

		// check if the commands in the line are all valid or not
		deque<string> commandList;
		int validationIndex = 0;
		bool someInvalidCommandFound = false;
		while (!commandList1.empty()) {
			string currCommand = commandList1.front();
			commandList1.pop_front();
			commandList.push_back(currCommand);
			if (!validCommand(currCommand, validationIndex, commandCount)) {
				someInvalidCommandFound = true;
				break;
			}
			validationIndex++;
		}
		if (someInvalidCommandFound) {
			printf("invalid input\n");
			fflush(stdout);
			continue;
		}	
		
		// create pid holders for the child processes
		int *childPids = new int[commandCount];

		// declare pipe descriptors for command linking
		int pipes[commandCount][2];

		// start processing the commands
		int commandNo = 0;
		while (!commandList.empty()) {

			string currCommand = commandList.front();
			commandList.pop_front();	

			// parse the command
			deque<string> tokenList = tokenizeString(currCommand, argDelim);

			// determine the command arguments and any input/output file redirection
			char *inputFile = NULL;
			char *outputFile = NULL;
			vector<string> argVector;			
			int argNo = 0;
			while (!tokenList.empty()) {
				string arg = tokenList.front();
				tokenList.pop_front();
				if (strcmp(arg.c_str(), ">") == 0) {
					outputFile = (char *) strdup(tokenList.front().c_str()); 
					tokenList.pop_front();
				} else if (strcmp(arg.c_str(), "<") == 0) {
					inputFile = (char *) strdup(tokenList.front().c_str());
					tokenList.pop_front();
				} else {
					argVector.push_back(arg);
					argNo++;
				}
			}

			// instantiate a pipe for the current command if it is not the only command in the line
			// and the current command is not the last in the line
			if (commandCount > 1 && commandNo < commandCount - 1) {
				pipe(pipes[commandNo]);
			}

			// create a new process for the current command
			int pid = fork();

			// child process
			if (pid == 0) {
			
				// check if there is any input redirection; if YES then redirect the stdin	
				if (inputFile != NULL && commandNo == 0) {
					int fileDescriptor = open(inputFile, O_RDONLY);
					if (fileDescriptor < 0) {
						cout << "Could not open input redirect file: " << inputFile << "\n";
						exit(EXIT_FAILURE);
					}
					dup2(fileDescriptor, STDIN_FILENO);
					close(fileDescriptor);
				}

				// check if there is any output redirection; if YES then redirect the stdout
				if (outputFile != NULL && commandNo == commandCount - 1) {
					int fileDescriptor = open(outputFile, O_CREAT | O_RDWR, 0666);
					if (fileDescriptor < 0) {
						cout << "Could not open output redirect file\n";
						exit(EXIT_FAILURE);
					}
					dup2(fileDescriptor, STDOUT_FILENO);
					close(fileDescriptor);
				}

				// if the current command is not the first command then it should attach its
				// stdin to the stdout of the previous command and vice-versa
				if (commandNo > 0) {
					// redirect input
					dup2(pipes[commandNo - 1][0], STDIN_FILENO);
					// close the write end of the pipe
					close(pipes[commandNo - 1][1]);
				}
				if (commandNo < commandCount - 1) {
					// redirect output
					dup2(pipes[commandNo][1], STDOUT_FILENO);
					// close the read end of the pipe
					close(pipes[commandNo][0]);
				}
				
				// create an argument array to pass the arguments to the linux command
				int argCount = argNo;
				char **args = new char*[argCount + 1];
				for (int i = 0; i < argVector.size(); i++) {
					args[i] = (char *) argVector[i].c_str();
				}
				args[argCount] = NULL;
				char *env[] = {NULL};

				// change the child environment to the environment of the command the user wants
				// to execute
				execve(args[0], args, env);

				// if the exec call fails then exit from the child prcess immediately
				cout << "child process failed!!!\n";
				exit(EXIT_FAILURE);

			// parent process
			} else {
				// store the child process PID to wait on to
				childPids[commandNo] = pid;
				
				// close pipes' ends that are no longer needed
				if (commandCount > 1) {
					// previous command's read/write end can be closed too now
					if (commandNo > 0) {
						close(pipes[commandNo - 1][0]);
						close(pipes[commandNo - 1][1]);
					}
				}
				
				// advance the command index
				commandNo++;
			}
		} 

		// wait for all child processes to finish
		int childExitStatuses[commandCount];
		for (int i = 0; i < commandCount; i++) {
			childExitStatuses[i] = 0;	
			waitpid(childPids[i], &childExitStatuses[i], WEXITED);
		}

		// print the exit codes of the childs
		for (int i = 0; i < commandCount; i++) {
			int exitStatus = childExitStatuses[i];
			fprintf(stderr, "%d\n", exitStatus);
		}
		fflush(stdout);
	}
	
	return 0;
}

void shrinkWhitespaces(string &str) {
        for (int i = 0; i < str.length(); i++) {
                if (!isspace(str[i])) continue;
                str.replace(i, 1, " ");
                int j = i + 1;
                while (j < str.length()) {
                        if (isspace(str[j])) {
                                str.erase(j, 1);
                        } else break;
                }
        }
}

deque<string> tokenizeString(string &str, string &delim) {
        deque<string> tokenList;
        string strCp;
        strCp.assign(str);
        size_t pos = 0;
        string token;
        while ((pos = strCp.find(delim)) != string::npos) {
                token = strCp.substr(0, pos);
                trim(token);
                shrinkWhitespaces(token);
                if (token.length() > 0) tokenList.push_back(token);
                strCp.erase(0, pos + delim.length());
        }
        trim(strCp);
        if (strCp.length() > 0) tokenList.push_back(strCp);
        return tokenList;
}

void trim(string &str) {

        // remove all white spaces at the beginning
        while (str.length() > 0) {
                if (isspace(str[0])) {
                        str.erase(0, 1);
                } else break;
        }
        // remove all white spaces at the end
        while (str.length() > 0) {
                if (isspace(str[str.length() - 1])) {
                        str.erase(str.length() - 1, 1);
                } else break;
        }
}

void readNonEmptyLine(string &line) {
        while (true) {
                getline(cin, line);
                trim(line);
                if (line.length() > 0) break;
        }
}

bool validLine(string line) {
	const char *chArray = line.c_str();
	int length = line.length();
	if (length > MAX_LINE_LENGTH) return false;					// line too long
	for (int i = 0; i < length; i++) {
		char ch = chArray[i];
		if (!((ch >= 'A' && ch <= 'Z') || ( ch >= 'a' && ch <= 'z') 		// letters
				|| (ch >= '0' && ch <= '9') 				// numbers
				|| ch == '.' || ch == '-' || ch == '_' || ch == '/'	// special characters
				|| ch == ' ' || ch == '\t'				// white spaces 
				|| ch == '<' || ch == '>' || ch == '|')) {		// operators
			return false;
		}
	}
	if (chArray[length - 1] == '|') return false;					// meaningless pipe at the end
	return true;
}

bool validCommand(string command, int index, int commandCount) {
	int length = command.length();
	if (length == 0) return false;					// an empty command within pipes
	const char *chArray = command.c_str();
	char firstCh = chArray[0];
	if (firstCh == '<' || firstCh == '>') return false;		// command starting with an operator
	if (index > 0) {
		string inputRedirect = "<"; 
		size_t found = command.find(inputRedirect);
		if (found != string::npos) return false;		// a later command having input redirect
	}
	if (index < commandCount - 1) {
		string outputRedirect = ">"; 
		size_t found = command.find(outputRedirect);
		if (found != string::npos) return false;		// an earlier command having output redirect

	}
	char lastCh = chArray[length - 1];
	if (lastCh == '<' || lastCh == '>' 
			|| (index == commandCount - 1 && lastCh == '|')) {
		return false;						// a meaningless redirect operator
	}
	return true;
}