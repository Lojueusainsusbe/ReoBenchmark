#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>
#include<pthread.h>


class Alternate {
	public:
		Alternate(int n) {
			data = (int*) malloc(n*sizeof(int));
			dataidx = 0;
			actors = n+1;

			// Barriers
			pthread_barrier_init(&alltriedbarrier, NULL, actors);
			pthread_barrier_init(&allputbarrier, NULL, actors);
		}
		~Alternate() {
			free(data);
		}

		// Corresponds to Reo putter
		void put(int id, int d) {
			pthread_barrier_wait(&alltriedbarrier);
			data[id] = d;
			pthread_barrier_wait(&allputbarrier);	
		}

		// Corresponds to Reo getter
		int get() {
			if(dataidx == 0) {
				pthread_barrier_wait(&alltriedbarrier);
			}

			// Make sure the items are accessed after they are written to
			if(dataidx == 0) {
				pthread_barrier_wait(&allputbarrier);	
			}
				
			int dat = data[dataidx++];
			
			// All the items have left the alternator	
			if(dataidx == actors-1) {
				dataidx = 0;
			}
			
			return dat;	
		}

	private:
		// Barriers
		pthread_barrier_t alltriedbarrier;
		pthread_barrier_t allputbarrier;

		int dataidx;
		int* data;
		int actors;
};


class Producer {
	public:
		Producer() {}
		void setMembers(int i, Alternate* a, int act) {
			id = i;
			alt = a;
			actions = act;
		}

		static void* prodcall(void* ptr) {
			(static_cast<Producer*>(ptr))->produce();
			return NULL;
		}

		void produce() {
			while(actions > 0) {
				alt->put(id, id);
				actions--;
			}
		}

	private:
		int id;
		int actions;
		Alternate* alt;
};


class Consumer {
	public:
		Consumer() {}
		void setMembers(Alternate* a, int act) {
			alt = a;
			actions = act;
		}

		static void* concall(void* ptr) {
			(static_cast<Consumer*>(ptr))->consume();
			return NULL;
		}

		void consume() {			
			while(actions > 0) {
				int data = alt->get();
				actions--;
			}
		}

	private:
		int actions;
		Alternate* alt;
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
