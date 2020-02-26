#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>
#include<pthread.h>

std::chrono::time_point<std::chrono::high_resolution_clock> tstart;
std::chrono::time_point<std::chrono::high_resolution_clock> tend;

// Mutex logging
std::mutex logmutex;
void log(std::string text) {
	logmutex.lock();
	std::cout << text << std::endl;
	logmutex.unlock();
}


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

		// Mutex functions to increment and decrement a counter that is used to see
		// if all prods and the con have accessed the alternator
		void announceArrival(int id) {
			log(std::to_string(id) + " announced arrival");
			pthread_barrier_wait(&alltriedbarrier);
			log(std::to_string(id) + " is now freeeee");	
		}

		// Corresponds to Reo putter
		void put(int id, int d) {
			announceArrival(id);

			log(std::to_string(id) + " now entering critical section");
			data[id] = d;
		
			pthread_barrier_wait(&allputbarrier);	
		}

		// Corresponds to Reo getter
		int get() {
			if(dataidx == 0) {
				announceArrival(-1);
			}

			// Make sure the items are accessed after they are written to
			if(dataidx == 0) {
				pthread_barrier_wait(&allputbarrier);	
			}
				
			log("A get is occuring");
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

		void produce() {
			while(actions > 0) {
				alt->put(id, id);
				actions--;
			}
			log("Producer " + std::to_string(id) + " shutting down");
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

		void consume() {
			while(actions > 0) {
				int data = alt->get();
				actions--;
			}

			tend = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = tend-tstart;

			log("Consumer shutting down after " + std::to_string(diff.count()) +" seconds");
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
	std::thread** threads = (std::thread**) malloc(N*sizeof(std::thread*));
	tstart = std::chrono::high_resolution_clock::now();
	
	for(int i = 0; i < N; i++) {
		producers[i].setMembers(i, &alt, productions);
		threads[i] = new std::thread(&Producer::produce, &producers[i]);
	}

	// Start consuming
	consumer.setMembers(&alt, productions * N);
	std::thread* conthread = new std::thread(&Consumer::consume, &consumer);	

	// Join all the threads
	conthread->join();
	for(int i = 0; i < N; i++) {	
		threads[i]->join();
		delete threads[i];
	}	

	delete conthread;
	free(threads);
}
