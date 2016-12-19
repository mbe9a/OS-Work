// Michael Eller mbe9a
// OS Machine Problem 2
// 29 Spetember 2016
// barrier.cpp

#include "barrier.h"

// default constructor
Barrier::Barrier()
{
	// this don't do nothin'
}

// constructor
Barrier::Barrier(int number_of_threads)
{
	// init variables
	counter = 0;
	max = number_of_threads;

	// mutex for locking out all other threads when in wait()
	sem_init(&mutex, 0, 1);

	// waiter binary semaphore to hold all waiting threads
	sem_init(&waiter, 0, 0);

	// binary semaphore to ensure that only one thread is released at a time --
	// to ensure that binary semaphore is not violated
	sem_init(&handshake, 0, 0);
}

// method used to wait on the barrier --
// threads will be released when the number of threads that
// have called wait() == max
void Barrier::wait()
{
	// block other threads
	sem_wait(&mutex);

	// increment counter
	counter++;

	// if all are waiting
	if (counter == max)
	{
		// reset counter
		counter = 0;

		// release the 'barrier'
		for (int i = 0; i < max - 1; i++) {
			// release a single thread
			sem_post(&waiter);

			// wait for the thread to be released --
			// essential so that the binary semaphore is not violated
			sem_wait(&handshake);
		}

		// unlock mutex
		sem_post(&mutex);

		return;
	}

	// unlock mutex and wait on waiter semaphore
	sem_post(&mutex);
	sem_wait(&waiter);

	// allow barrier to continue releasing and exit
	sem_post(&handshake);

	return;
}