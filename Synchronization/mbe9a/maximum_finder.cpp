// Michael Eller mbe9a
// OS Machine Problem 2
// 29 Spetember 2016
// maximum_finder.cpp

#include "maximum_finder.h"

// thread function to calculate the maximum of the input numbers
void* find_maximum (void* input)
{	
	// cast input into pthread_params
	struct pthread_params* parameters;
	parameters = (pthread_params*) input;

	// main loop, runs once per round
	for (int i = 0; i < parameters->rounds; i++)
	{
		// make sure only 'valid' threads continue comparing and adding to the next round
		if (parameters->tid < (*parameters->results)[i].size()/2)
		{
			// get values from current vector
			parameters->value_1 = (*parameters->results)[i][2*parameters->tid];
			parameters->value_2 = (*parameters->results)[i][2*parameters->tid + 1];

			// determine which value is larger
			if (parameters->value_1 > parameters->value_2)
			{
				parameters->result = parameters->value_1;
			}
			else
			{
				parameters->result = parameters->value_2;
			}

			// add result to next vector
			(*parameters->results)[i + 1][parameters->tid] = parameters->result;
		}
		
		// wait on all threads before the next round
		parameters->barrier->wait();
	}

	// kill thread when done
	pthread_exit(NULL);
}

int main (int argc, char const *argv[])
{
	// list to hold the numbers
	vector<int> numbers;

	// read input, stop if blank line is received
	char *buffer = (char*)malloc(sizeof(char)*128);
	while(fgets(buffer, 128, stdin)[0] != '\n')
	{	
		// use atoi() to cast to int and push to the vector list
		numbers.push_back(atoi(buffer));
	}

	// get number of threads to create
	unsigned int number_of_threads = numbers.size()/2;

	// calculate number of rounds based on the number of inputs -- assuming powers of 2
	int rounds = log2(numbers.size());

	// make nested vector for the results of each round
	vector<vector<int> > results;
	results.push_back(numbers);
	for (int i = 1; i < rounds + 1; i++)
	{
		// must initialize vectors with size to prevent segfaults
		vector<int> temp (numbers.size()/(2*i), 0);
		results.push_back(temp);
	}

	// create the barrier for synchronization
	Barrier barrier = Barrier(number_of_threads);

	// create paramters list
	pthread_params pthread_params_array[number_of_threads];

	// create threads list
	pthread_t pthread_array[number_of_threads];

	// create pthread attr
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);

	// create the threads and thread parameter structs
	for (int i = 0; i < number_of_threads; i++)
	{
		// value_1 and value_2 are set in find_maximum()
		pthread_params_array[i].rounds = rounds;
		pthread_params_array[i].tid = i;
		pthread_params_array[i].results = &results;
		pthread_params_array[i].barrier = &barrier;
		pthread_create(&pthread_array[i], &thread_attr, find_maximum, (void*) &pthread_params_array[i]);
	}

	// join on all threads
	for (int i = 0; i < number_of_threads; i++)
	{
		pthread_join(pthread_array[i], NULL);
	}

	// final result is filtered down to the first thread's result
	// display final result to stdin and exit
	printf("%d\n", pthread_params_array[0].result);

	return 0;
}