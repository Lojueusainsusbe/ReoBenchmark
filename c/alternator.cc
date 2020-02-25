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
			// Allocate stuff
			actors = n+1;
			locks = (pthread_mutex_t**) malloc(n*sizeof(pthread_mutex_t*));
			for(int i = 0; i < n; i++) {
				locks[i] = new pthread_mutex_t;
				*locks[i] = PTHREAD_MUTEX_INITIALIZER;
			}
			data = (int*) malloc(n*sizeof(int));
			
			// Init members
			tried = 0;
			putted = 0;
			dataidx = 0;

			// Mutexes and conditions
			alltried = PTHREAD_COND_INITIALIZER;
			allput = PTHREAD_COND_INITIALIZER;
			allputlock = PTHREAD_MUTEX_INITIALIZER;
		}

		// Mutex functions to increment and decrement a counter that is used to see
		// if all prods and the con have accessed the alternator
		void announceArrival(int id) {
			log(std::to_string(id) + " announced arrival");
			triedlock.lock();
			tried++;
			log("TRIED: " + std::to_string(tried) + ", PUT: " + std::to_string(putted));

			// All parties go at once in an alternator
			if(tried == actors && putted == 0) {
				log(std::to_string(id) + " broadcasting that producers may put");
				pthread_cond_broadcast(&alltried);
			}
			else if(id >= 0)  { // Wait for all other threads
				int lock = 0;
				if((lock = pthread_mutex_lock(locks[id])) != 0 ) {
					log("ERRRRRRRRRRRRRRRRRRRR: " + std::to_string(lock));
				}
				log(std::to_string(id) + " now waiting");
				triedlock.unlock();
				pthread_cond_wait(&alltried, locks[id]);
				pthread_mutex_unlock(locks[id]);
				return;
			}

			triedlock.unlock();
		}
		void announcePut(int id) {
			log(std::to_string(id) + " announced put");
			
			triedlock.lock();
			tried--;
			triedlock.unlock();
			
			putlock.lock();
			putted++;
		
			log(std::to_string(id) + " PUT: " + std::to_string(putted));

			// All data is consumed when it is all put
			if(putted == actors-1) {
				log("Signaling PUUUUUUUUUUUUUUTS");
				pthread_cond_signal(&allput);
			} 

			putlock.unlock();	
		}

		// Corresponds to Reo putter
		void put(int id, int d) {
			announceArrival(id);

			log(std::to_string(id) + " now entering critical section");
			data[id] = d;
			announcePut(id);
		}

		// Corresponds to Reo getter
		int get() {
			announceArrival(-1);

			// Make sure the items are accessed after they are written to
			if(dataidx == 0 && !(putted == actors-1)) {
				pthread_mutex_lock(&allputlock);
				pthread_cond_wait(&allput, &allputlock);
				pthread_mutex_unlock(&allputlock);
				log("All put, getter getting");
			}
	
			log("A get is occuring");
			int dat = data[dataidx++];
			putlock.lock();
			putted--;
			putlock.unlock();

			// All the items have left the alternator	
			if(dataidx == actors-1) {
				dataidx = 0;
			}
			
			triedlock.lock();
			tried--;
			triedlock.unlock();
			
			return dat;	
		}

	private:
		// Mutexes for blocking
		pthread_cond_t alltried; 
		pthread_cond_t allput;
		pthread_mutex_t allputlock;
	
		// Mutexes for counters	
		std::mutex triedlock;
		std::mutex putlock;
		
		// Counters
		int tried;
		int putted;
		int dataidx;

		int* data;
		int actors;
		pthread_mutex_t** locks;
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
				log("DATA: " + std::to_string(data));

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
	}	

}
