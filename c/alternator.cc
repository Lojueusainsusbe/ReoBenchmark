#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>


class Alternate {
	public:
		Alternate(int n) {
			actors = n+1;
			locks = (std::mutex**) malloc((actors-1)*sizeof(std::mutex*));
			for(int i = 0; i < actors-1; i++) {
				locks[i] = new std::mutex;
				locks[i]->lock();
			}
			tried = 0;
			dataidx = 0;
			data = (int*) malloc(sizeof(int));
		}

		void announceArrival() {	
			triedlock.lock();
			tried++;
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
			announceArrival();

			locks[id]->lock();
			data[id] = d;
		
			announceDeparture();
			locks[id]->unlock();
		}


		// Corresponds to Reo getter
		int get() {
			announceArrival();
			int dat = data[dataidx++];
			if(dataidx == actors-2) {
				//reset
				dataidx = 0;
			}
				
			announceDeparture();
			return dat;	
		}


		void unlock_all() {
			for(int i = 0; i < actors-1; i++) {
				locks[i]->unlock();
			}
			tried = 0;
		}
	private:
		std::mutex triedlock;
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
				alt->put(id, rand());	
				std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
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
				std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
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
