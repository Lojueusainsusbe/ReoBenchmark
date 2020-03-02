#include<stdio.h>
#include<iostream>
#include<thread>
#include<string>

#include "eaoseq.h"

EAOSequence::EAOSequence(int num) {
	N = num;
	turn = 0;

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
	// if(turn != 0) {
		pthread_mutex_lock(&buf_lock);
		pthread_cond_wait(&buf_cond, &buf_lock);
	// }

  buff = turn;

	// if(turn != 0)
		pthread_mutex_unlock(&buf_lock);

	// if(turn == 0)
	 // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	pthread_cond_signal(&c_conds[(turn++)%N]);
}

int EAOSequence::get(int id) {
  pthread_mutex_lock(&c_locks[id]);

	if(turn == 0 && id == 0)					//first getter comes here after receiving a signal
		pthread_cond_signal(&buf_cond); //unstable solution

	pthread_cond_wait(&c_conds[id], &c_locks[id]);

  int temp = buff;
	std::cerr << "C-" << id << ": " << temp << std::endl;

	pthread_mutex_unlock(&c_locks[id]);
	pthread_cond_signal(&buf_cond);

  return temp;
}
