#ifndef alternator_h
#define alternator_h

#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>
#include<pthread.h>

#include "protocol.h"


class Alternate : public Protocol {
	public:
		Alternate(int n);
		~Alternate();

		// Corresponds to Reo putter
		void put(int id, int d) override;
		// Corresponds to Reo getter
		int get() override;

	private:
		// Barriers
		pthread_barrier_t alltriedbarrier;
		pthread_barrier_t allputbarrier;

		int dataidx;
		int* data;
		int actors;
};

#endif
