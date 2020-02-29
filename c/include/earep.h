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
#include "consumer.h"
#include "producer.h"

class EARep : public Protocol{
	public:
		EARep(int n);

		void put(int data, int id) override;

		int get() override;

	private:
		pthread_mutex_t putlock;
		pthread_barrier_t conbarrier;
		pthread_cond_t putcond;


		int buff;
		int N;
		bool accepting;
};

#endif
