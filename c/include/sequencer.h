#ifndef sequence_h
#define sequence_h

#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>

#include "protocol.h"


class Protocol;

class Sequence : public Protocol {
	public:
		Sequence(int num);
		~Sequence();

		// Producer id does a put on the protocol
		void put(int id, int *data) override;

		// This protocol does not have outputs
		int* get(int id) override;
	private:
		pthread_mutex_t** locks;
		pthread_cond_t** conds;
		int N;
		int turn;
};

#endif
