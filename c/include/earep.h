#ifndef earep_h
#define earep_h

#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>
#include<pthread.h>

#include "protocol.h"

class EARep : public Protocol{
	public:
		EARep(int n);
		~EARep();

		void put(int id, int* data) override;

		int* get(int id) override;

	private:
		//Barrier
		pthread_barrier_t conbarrier;

		//Conditions
		pthread_cond_t concond;
		pthread_cond_t prodcond;

		//Locks
		pthread_mutex_t** conlocks;
		pthread_mutex_t prodlock;
		pthread_mutex_t countlock;

		int* buff;
		int N, count;
		bool fifo;
};

#endif
