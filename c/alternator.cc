#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>

class Producer {
	public:
		Producer() {}
		void setMembers(int i, std::mutex* l, int pc, int* b) {
			id = i;
			locks = l;
			prodCount = pc;
			buff = b;
		}

		void produce() {
			// Randomize when consumers start
			std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%prodCount)));
			while(true) {
				// Production
				locks[id].lock();
				std::cout << id << " is now producing" << std::endl;
				*buff = rand();
				
				// Unluck so the next can produce
				locks[(id+1)%prodCount].unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
	private:
		int id;
		std::mutex* locks;
		int prodCount;
		int* buff;
};

class Alternate() {
	public:
		Alternate(int n) {
			prodCount = n;
			locks = (std::mutex*) malloc(prodCount*sizeof(std::mutex));
			for(int i = 0; i < prodCount; i++) {
				locks[i] = new std::mutex;
				locks[i].lock();
			}
			tried = 0;
			got = false;
		}

		// Corresponds to Reo putter
		void put(int id, int d) {
			tried++;
			if(tried == prodCount && got) {
				unlock_all();
			}
			
			locks[id].lock();
			data[id] = d;
		}


		// Corresponds to Reo getter
		int get() {
			got = true;

			
		}


		void unlock_all() {
			for(int i = 0; i < prodCount; i++) {
				locks[i].unlock();
			}
		}
	private:
		int tried;
		bool got;
		std::mutex getlock;
		int* data;
		std::mutex* locks;
		int prodCount;
}


int main(int argc, char** argv) {
	if(argc < 2) { std::cout << "Give a number of producers" << std::endl; return -1; }
	int N = strtol(argv[1], NULL, 10);

	int buff;
	Producer producers[N];
	std::mutex locks[N];
	// Lock all except first
	for(int i = 1; i < N; i++) locks[i].lock();	

	// Start producing
	std::thread** threads = (std::thread**) malloc(N*sizeof(std::thread*));
	for(int i = 0; i < N; i++) {
		producers[i].setMembers(i, &locks[0], N, &buff);
		threads[i] = new std::thread(&Producer::produce, &producers[i]);
	}

	// Join all the threads
	for(int i = 0; i < N; i++) {	
		threads[i]->join();
	}	

}
