#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>

class Sequence {
	public:
		Sequence(int num) {
			N = num;
			
			// Create mutexes
			locks = (std::mutex**) malloc(N * sizeof(std::mutex*));
			for(int i = 0; i < N; i++) {
				locks[i] = new std::mutex;

				// Lock all except first
				if(i != 0) {
					locks[i]->lock();
				}
			}
		}

		// Producer id does a put on the protocol
		void put(int id, int data) {
			// Try to acquire your own lock
			locks[id]->lock();
			
			std::cout << id << " is now producing" << std::endl;
			buff = data;
			locks[(id+1)%N]->unlock();
		}

		// This protocol does not have outputs
		void get() {}
	private:
		std::mutex** locks;
		int N;
		int buff;
};


class Producer {
	public:
		Producer() {}
		void setMembers(int i, Sequence* s) {
			id = i;
			seq = s;
		}

		void produce() {
			// Randomize when consumers start
			std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
			while(true) {
				// Production
				seq->put(id, rand());
				std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
			}
		}
	private:
		int id;
		Sequence* seq;
};


int main(int argc, char** argv) {
	if(argc < 2) { std::cout << "Give a number of producers" << std::endl; return -1; }
	int N = strtol(argv[1], NULL, 10);

	Producer producers[N];
	Sequence seq(N);

	// Start producing
	std::thread** threads = (std::thread**) malloc(N*sizeof(std::thread*));
	for(int i = 0; i < N; i++) {
		producers[i].setMembers(i, &seq);
		threads[i] = new std::thread(&Producer::produce, &producers[i]);
	}

	// Join all the threads
	for(int i = 0; i < N; i++) {	
		threads[i]->join();
	}	

}
