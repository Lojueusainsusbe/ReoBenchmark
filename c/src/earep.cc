#include "earep.h"

EARep::EARep(int n) {
	N = n;
	count = 0;
	fifo = false;

	//Barrier
	pthread_barrier_init(&conbarrier, NULL, N);

	//Locks
	conlocks = (pthread_mutex_t**) malloc(N * sizeof(pthread_mutex_t*));
	for(int i = 0; i < N; i++) {
		conlocks[i] = new pthread_mutex_t;
		*conlocks[i] = PTHREAD_MUTEX_INITIALIZER;
	}
	prodlock = PTHREAD_MUTEX_INITIALIZER;
	countlock = PTHREAD_MUTEX_INITIALIZER;

	//Conditions
	prodcond = PTHREAD_COND_INITIALIZER;
	concond = PTHREAD_COND_INITIALIZER;
}

EARep::~EARep() {
	for(int i = 0; i < N; i++) {
		delete conlocks[i];
	}
	free(conlocks);
}

void EARep::put(int id, int data) {
	// Check whether fifo is full (fifo = true),
	// in other words whether its your turn
	pthread_mutex_lock(&prodlock);
	if(fifo) {
		pthread_cond_wait(&prodcond, &prodlock);
	}
	pthread_mutex_unlock(&prodlock);

	// Critical section
	buff = data;

	// Reset counter
	pthread_mutex_lock(&countlock);
	count = N;
	pthread_mutex_unlock(&countlock);

	// Set fifo to full (fifo = true), pass turn to consumers
	for(int i = 0; i < N; i++) {
		pthread_mutex_lock(conlocks[i]);
	}
	pthread_cond_broadcast(&concond);
	fifo = true;
	for(int i = 0; i < N; i++) {
		pthread_mutex_unlock(conlocks[i]);
	}
}

int EARep::get(int id) {
	int temp;

	// Check whether fifo is empty (fifo = false),
	// in other words whether its your turn
	pthread_mutex_lock(conlocks[id]);
	if(!fifo) {
		pthread_cond_wait(&concond, conlocks[id]);
	}
	pthread_mutex_unlock(conlocks[id]);

	// All consumers wait for each other
	pthread_barrier_wait(&conbarrier);

	// Lower count for every production
	pthread_mutex_lock(&countlock);
	count--;
	temp = count;
	pthread_mutex_unlock(&countlock);

	// If last get, empty fifo and pass turn to producer
	if(temp == 0) {
		pthread_mutex_lock(&prodlock);
		pthread_cond_signal(&prodcond);
		fifo = false;
		pthread_mutex_unlock(&prodlock);
	}

	return buff;
}
