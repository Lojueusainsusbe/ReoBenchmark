#include<stdio.h>
#include<iostream>
#include<thread>
#include<string>

#include "eaoseq.h"
#include "mutexprint.h"

EAOSequence::EAOSequence(int num) {
	N = num;
	turn = 0;
	full = false;

	// Create mutexes and conditions
  	c_locks = new pthread_mutex_t[N];
	c_conds = new pthread_cond_t[N];

	for(int i = 0; i < N; i++) {
		c_conds[i] = PTHREAD_COND_INITIALIZER;
		c_locks[i] = PTHREAD_MUTEX_INITIALIZER;
	}

	buf_cond = PTHREAD_COND_INITIALIZER;
	buf_lock = PTHREAD_MUTEX_INITIALIZER;
}

EAOSequence::~EAOSequence() {
	delete[] c_locks;
}

// Producer id does a put on the protocol
void EAOSequence::put(int id, int data) {
	pthread_mutex_lock(&buf_lock);
	if(full) {
		pthread_cond_wait(&buf_cond, &buf_lock);
	}
	pthread_mutex_unlock(&buf_lock);

	int next = (turn + 1) % N;

	pthread_mutex_lock(&c_locks[next]);

	buff = data;
	turn = next;
	full = true;

	pthread_mutex_unlock(&c_locks[next]);
	pthread_cond_signal(&c_conds[next]);
}

int EAOSequence::get(int id) {
	pthread_mutex_lock(&c_locks[id]);
	if(id != turn || !full) {
    	pthread_cond_wait(&c_conds[id], &c_locks[id]);
	}
    pthread_mutex_unlock(&c_locks[id]);

	pthread_mutex_lock(&buf_lock);

  	int temp = buff;
	full = false;

	pthread_mutex_unlock(&buf_lock);
	pthread_cond_signal(&buf_cond);

  return temp;
}
