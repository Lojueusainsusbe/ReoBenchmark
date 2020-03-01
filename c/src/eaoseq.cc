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
	for(int i = 0; i < N; i++) {
		c_locks[i] = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&c_locks[i]);
	}
}

EAOSequence::~EAOSequence() {
	delete[] c_locks;
}

// Producer id does a put on the protocol
void EAOSequence::put(int id, int data) {
  pthread_mutex_lock(&buf_lock);

  buff = turn;
  
  pthread_mutex_unlock(&c_locks[(turn++)%N]);
}

int EAOSequence::get(int id) {
  pthread_mutex_lock(&c_locks[id]);

  int temp = buff;

  pthread_mutex_unlock(&buf_lock);

  return temp;
}
