#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include <string>

class Alternate {
	public:
		Alternate(int n) {
			// Allocate stuff
			locks = (std::mutex**) malloc((actors-1)*sizeof(std::mutex*));
			for(int i = 0; i < actors-1; i++) {
				locks[i] = new std::mutex;
				locks[i]->lock();
			}
			data = (int*) malloc(sizeof(int));
			
			// Init members
			actors = n+1;
			tried = 0;
			dataidx = 0;
			firstlock.lock();
			accepting = true;
		}

		// Mutex functions to increment and decrement a counter that is used to see
		// if all prods and the con have accessed the alternator
		void announceArrival(int id) {
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

		// Corresponds to Reo putter
		void put(int id, int d) {
			announceArrival(id);

			locks[id]->lock();
			data[id] = d;
			locks[id]->unlock();
			
			if(id == 0) {
				firstlock.unlock();
			}
		
			announceDeparture();
		}

		// Corresponds to Reo getter
		int get() {
			// Only announce arrival when not in emptying phase
			if(accepting) {
				announceArrival(-1);
			}

			// Make sure the first item is accessed after it is written to
			if(dataidx == 0) {
				firstlock.lock();
			}
	
			log("Consumer taking data item " + std::to_string(dataidx));
			int dat = data[dataidx++];

			// All the items have left the alternator	
			if(dataidx == actors-1) {
				log("All items got, resetting");
				dataidx = 0;
				accepting = true;
			}
			
			announceDeparture();
			return dat;	
		}


		void unlock_all() {
			log("Unlocking all");
			for(int i = 0; i < actors-1; i++) {
				locks[i]->unlock();
			}
			accepting = false;
		}
	private:
		bool accepting;
		std::mutex triedlock;
		std::mutex firstlock;
		int tried;
		int* data;
		int dataidx;
		std::mutex** locks;
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
			// Randomize when consumers start
			std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
			while(true) {
				alt->put(id, id);	
				std::this_thread::sleep_for(std::chrono::milliseconds(200*(rand()%50)));
			}
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
			while(true) {
				int data = alt->get();
				std::cout << data << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(2*(rand()%50)));
			}	
		}
	private:
		Alternate* alt;
};


int main(int argc, char** argv) {
	if(argc < 2) { std::cout << "Give a number of producers" << std::endl; return -1; }
	int N = strtol(argv[1], NULL, 10);

	Producer producers[N];
	Consumer consumer;
	Alternate alt(N);

	// Start producing
	std::thread** threads = (std::thread**) malloc(N*sizeof(std::thread*));
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
