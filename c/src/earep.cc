#include "../include/earep.h"

EARep::EARep(int n){
	N = n;
	pthread_barrier_init(&conbarrier, NULL, N);
	accepting = true;
	putlock = PTHREAD_MUTEX_INITIALIZER;
	putcond = PTHREAD_COND_INITIALIZER;
}

void EARep::put(int data, int id) {
	pthread_mutex_lock(&putlock);
	if(!accepting) {
		//std::cout << "put is waiting" << std::endl;
		pthread_cond_wait(&putcond, &putlock);
	}
	pthread_mutex_unlock(&putlock);
	accepting = false;
	buff = data;
	//std::cout << "put" << std::endl;
}

int EARep::get() {
	//std::cout << "get before wait" << std::endl;
	pthread_barrier_wait(&conbarrier);
	//std::cout << "get after wait" << std::endl;

	if(!accepting) accepting = true;
	else pthread_cond_signal(&putcond);

	//std::cout << "get" << std::endl;
	return buff;
}

int main(int argc, char** argv) {
    if(argc < 3) { std::cout << "Give a number of consumers and an action count" << std::endl; return -1; }
    int N = strtol(argv[1], NULL, 10);
    int productions = strtol(argv[2], NULL, 10);

    Producer producer;
    Consumer consumers[N];
    EARep ear(N);

		// Start producing
		auto tstart = std::chrono::high_resolution_clock::now();

    producer.setMembers(-1, &ear, productions);
    pthread_t prodthread;
    pthread_create(&prodthread, NULL, &Producer::call, &producer);

    // Start consuming
    pthread_t** threads = (pthread_t**) malloc(N*sizeof(pthread_t*));

    for(int i = 0; i < N; i++) {
        threads[i] = new pthread_t;
        consumers[i].setMembers(i, &ear, productions);
        pthread_create(threads[i], NULL, &Consumer::call, &consumers[i]);
    }

    // Join all the threads
    pthread_join(prodthread, NULL);
    for(int i = 0; i < N; i++) {
        pthread_join(*threads[i], NULL);
    }

    // Timing
    auto tend = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = tend-tstart;
    std::cout << diff.count() << std::endl;

    // Cleanup
    for(int i = 0; i < N; i++) {
        delete threads[i];
    }
    free(threads);
}
