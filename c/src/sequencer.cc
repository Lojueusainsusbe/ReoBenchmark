#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>

#include "sequencer.h"

Sequence::Sequence(int num) {
	N = num;
	turn = 0;

	// Create mutexes and conditions
	conds = (pthread_cond_t**) malloc(N * sizeof(pthread_cond_t**));
	locks = (pthread_mutex_t**) malloc(N * sizeof(pthread_mutex_t**));
	for(int i = 0; i < N; i++) {
		conds[i] = new pthread_cond_t;
		locks[i] = new pthread_mutex_t;
		*conds[i] = PTHREAD_COND_INITIALIZER;
		*locks[i] = PTHREAD_MUTEX_INITIALIZER;
	}
}

Sequence::~Sequence() {
	for(int i = 0; i < N; i++) {
		delete conds[i];
		delete locks[i];
	}
	free(conds);
	free(locks);
}

// Producer id does a put on the protocol
void Sequence::put(int id, int data) {
	// Atomically check whether its your turn
	pthread_mutex_lock(locks[id]);
	if(id != turn) {
		pthread_cond_wait(conds[id], locks[id]);
		turn = id;
	}
	pthread_mutex_unlock(locks[id]);
	
	// Block next producer
	int next = (id + 1) % N;
	pthread_mutex_lock(locks[next]);
	
	// Critical section
	buff = data;
	
	// Pass turn to the next producer
	turn = next;
	pthread_cond_signal(conds[next]);
	pthread_mutex_unlock(locks[next]);
}

int Sequence::get() {
	return 0;
}
