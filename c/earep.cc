#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>
#include<pthread.h>

class EARep {
	public:
		EARep(int n) {
			pthread_barrier_init(&conbarrier, NULL, N);	
			N = n;
			accepting = true;
			putlock = PTHREAD_MUTEX_INITIALIZER;
			putcond = PTHREAD_COND_INITIALIZER;
		}

		void put(int data, int id) {
			pthread_mutex_lock(&putlock);
			if(!accepting) {
				pthread_cond_wait(&putcond, &putlock);
			}
			pthread_mutex_unlock(&putlock);
			buff = data;
				
		}

		int get() {
			pthread_mutex_lock(&putlock);
			pthread_barrier_wait(conbarrier);
			return buff;
		}
	private:
		pthread_mutex_t putlock;
		pthread_barrier_t conbarrier;
		pthread_cond_t putcond;
		
		
		int buff;
		int N;
		bool accepting;
};



int main(int argc, char** argv) {
    if(argc < 3) { std::cout << "Give a number of producers and an action count" << std::endl; return -1; }
    int N = strtol(argv[1], NULL, 10);
    int productions = strtol(argv[2], NULL, 10);

    Producer producers[N];
    Consumer consumer;
    Alternate alt(N);

    // Start producing
    pthread_t** threads = (pthread_t**) malloc(N*sizeof(pthread_t*));

    auto tstart = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; i++) {
        threads[i] = new pthread_t;
        producers[i].setMembers(i, &alt, productions);
        pthread_create(threads[i], NULL, &Producer::prodcall, &producers[i]);
    }

    // Start consuming
    consumer.setMembers(&alt, productions * N);
    pthread_t conthread;
    pthread_create(&conthread, NULL, &Consumer::concall, &consumer);

    // Join all the threads
    pthread_join(conthread, NULL);
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
