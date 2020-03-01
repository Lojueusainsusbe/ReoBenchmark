#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>
#include<pthread.h>

#include "alternator.h"

Alternate::Alternate(int n) {
	data = (int*) malloc(n*sizeof(int));
	dataidx = 0;
	actors = n+1;

	// Barriers
	pthread_barrier_init(&alltriedbarrier, NULL, actors);
	pthread_barrier_init(&allputbarrier, NULL, actors);
}

Alternate::~Alternate() {
	free(data);
}


// Corresponds to Reo putter
void Alternate::put(int id, int d) {
	pthread_barrier_wait(&alltriedbarrier);
	data[id] = d;
	pthread_barrier_wait(&allputbarrier);
}

// Corresponds to Reo getter
int Alternate::get(int id) {
	if(dataidx == 0) {
		pthread_barrier_wait(&alltriedbarrier);
	}

	// Make sure the items are accessed after they are written to
	if(dataidx == 0) {
		pthread_barrier_wait(&allputbarrier);
	}

	int dat = data[dataidx++];

	// All the items have left the alternator
	if(dataidx == actors-1) {
		dataidx = 0;
	}

	return dat;
}
