// Michael Eller mbe9a
// OS Machine Problem 2
// 29 Spetember 2016
// maximum_finder.h

#ifndef MAXIMUM_FINDER_H
#define MAXIMUM_FINDER_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include "barrier.h"
using namespace std;

// struct used to hold information for individual threads
struct pthread_params
{
	// values hold the numbers to compare for the round
	int value_1;
	int value_2;

	// max(value_1, value_2) goes in result
	int result;

	// thread id, used for indexing
	int tid;

	// total number of rounds to perform
	int rounds;

	// 2D vector pointer for winners of each round
	vector<vector<int> >* results;

	// barrier pointer, sychronization object
	Barrier* barrier;
};

// thread function to calculate the higher of the 
// two current values in the thread params struct
void* find_maximum (void* input);

#endif