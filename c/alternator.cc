#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>
#include<pthread.h>

int actions = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> tstart;
std::chrono::time_point<std::chrono::high_resolution_clock> tend;


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
			locks = (pthread_mutex_t**) malloc((actors-1)*sizeof(pthread_mutex_t*));
			for(int i = 0; i < actors-1; i++) {
				locks[i] = new pthread_mutex_t;
				*locks[i] = PTHREAD_MUTEX_INITIALIZER;
			}
			data = (int*) malloc(sizeof(int));
			
			// Init members
			tried = 0;
			putted = 0;
			dataidx = 0;
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

			// All parties go at once in an alternator
			if(tried == actors) {
				unlock_all();
			}
			
			triedlock.unlock();
		}
		void announceDeparture() {	
			triedlock.lock();
			tried--;
			triedlock.unlock();
		}
		void announcePut() {
			putlock.lock();
			putted++;
		
			log("PUT: " + std::to_string(putted));

			// All data is consumed when it is all put
			if(putted == actors) {
				pthread_cond_signal(&allput);
			}

			putlock.unlock();	
		}

		// Corresponds to Reo putter
		void put(int id, int d) {
			announceArrival(id);

			pthread_mutex_lock(locks[id]);
			pthread_cond_wait(&alltried, locks[id]);

			log(std::to_string(id) + " now entering critical section");
			data[id] = d;	
			announcePut();

			pthread_mutex_unlock(locks[id]);
		
			announceDeparture();
		}

		// Corresponds to Reo getter
		int get() {
			// Only announce arrival when not in emptying phase
			if(dataidx == 0) {
				announceArrival(-1);
			}

			// Make sure the items ire accessed after they are written to
			if(dataidx == 0) {
				pthread_mutex_lock(&allputlock);
				pthread_cond_wait(&allput, &allputlock);
				pthread_mutex_unlock(&allputlock);
			}
	
			int dat = data[dataidx++];
			putlock.lock();
			putted--;
			putlock.unlock();

			// All the items have left the alternator	
			if(dataidx == actors-1) {
				dataidx = 0;
			}
			
			announceDeparture();
			return dat;	
		}


		void unlock_all() {
			log("now broadcasting");
			pthread_cond_broadcast(&alltried);
		}

	private:
		pthread_cond_t alltried; 
		pthread_cond_t allput;
		int tried;
		int putted;
		pthread_mutex_t allputlock;
		
		std::mutex triedlock;
		std::mutex putlock;
		int* data;
		int dataidx;
		pthread_mutex_t** locks;
		int actors;
};


class Producer {
	public:
		Producer() {}
		void setMembers(int i, Alternate* a) {
			id = i;
			alt = a;
		}

		void produce() {
			while(actions != 0) {
				alt->put(id, id);	
			}
			exit(0);
		}

	private:
		int id;
		Alternate* alt;
};


class Consumer {
	public:
		Consumer() {}
		void setMembers(Alternate* a) {
			alt = a;
		}

		void consume() {
			while(actions >= 0) {
				int data = alt->get();
				log("DATA: " + std::to_string(data));
				actions--;
			}

			tend = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = tend-tstart;

			std::cout << diff.count() << std::endl;
			exit(0);
		}
	private:
		Alternate* alt;
};


int main(int argc, char** argv) {
	if(argc < 3) { std::cout << "Give a number of producers and an action count" << std::endl; return -1; }
	int N = strtol(argv[1], NULL, 10);
	actions = strtol(argv[2], NULL, 10);

	Producer producers[N];
	Consumer consumer;
	Alternate alt(N);

	// Start producing
	std::thread** threads = (std::thread**) malloc(N*sizeof(std::thread*));
	tstart = std::chrono::high_resolution_clock::now();
	
	for(int i = 0; i < N; i++) {
		producers[i].setMembers(i, &alt);
		threads[i] = new std::thread(&Producer::produce, &producers[i]);
	}

	// Start consuming
	consumer.setMembers(&alt);
	std::thread* conthread = new std::thread(&Consumer::consume, &consumer);	

	// Join all the threads
	conthread->join();
	for(int i = 0; i < N; i++) {	
		threads[i]->join();
	}	

}
