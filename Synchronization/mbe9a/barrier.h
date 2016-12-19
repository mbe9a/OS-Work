// Michael Eller mbe9a
// OS Machine Problem 2
// 29 Spetember 2016
// barrier.h

#ifndef BARRIER_H
#define BARRIER_H

#include <semaphore.h>
using namespace std;

class Barrier {
public:
	// 1 mutex and 2 binary semaphores will be used
	sem_t mutex, waiter, handshake;

	// counter to hold the number of waiting threads, int for maximum number of threads to wait
	int counter, max;

	// default constructor
	Barrier();

	// constructor
	Barrier(int number_of_threads);

	// only method implemented, called to wait just like the pthread_barrier
	void wait();
};

#endif