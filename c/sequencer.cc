#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>

class Sequence {
	public:
		Sequence(int num) {
			N = num;
			
			// Create mutexes and conditions
			conds = (pthread_cond_t**) malloc(N * sizeof(pthread_cond_t**));
			locks = (pthread_mutex_t**) malloc(N * sizeof(pthread_mutex_t**));
			for(int i = 0; i < N; i++) {
				conds[i] = new pthread_cond_t;
				locks[i] = new pthread_mutex_t;
				*conds[i] = PTHREAD_COND_INITIALIZER;
				*locks[i] = PTHREAD_MUTEX_INITIALIZER;
			}
		}
		~Sequence() {
			for(int i = 0; i < N; i++) {
				delete conds[i];
			}
			free(conds);
		}

		// Producer id does a put on the protocol
		void put(int id, int data) {
			// Wait if its not your turn
			if(turn != id) {
				pthread_mutex_lock(locks[id]);
				pthread_cond_wait(conds[id], locks[id]);
				pthread_mutex_unlock(locks[id]);
			}
			
			buff = data;

			// Signal that the next producer may go
			pthread_cond_signal(conds[(id+1)%N]);
			turn++;
		}

		// This protocol does not have outputs
		void get() {}
	private:
		pthread_mutex_t** locks;
		pthread_cond_t** conds;
		int N;
		int turn;
		int buff;
};


class Producer {
    public:
        Producer() {}
        void setMembers(int i, Sequence* s, int act) {
            id = i;
            seq = s;
            actions = act;
        }

        void produce() {
			while(actions > 0) {
                seq->put(id, id);
                actions--;
            }
        }

    private:
        int id;
        int actions;
       	Sequence* seq;
};


int main(int argc, char** argv) {
	if(argc < 3) { std::cout << "Give a number of producers and an action count" << std::endl; return -1; }
    int N = strtol(argv[1], NULL, 10);
    int productions = strtol(argv[2], NULL, 10);
	
	Producer producers[N];
	Sequence seq(N);

	// Start producing
	std::thread** threads = (std::thread**) malloc(N*sizeof(std::thread*));
	
	auto tstart = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < N; i++) {
		producers[i].setMembers(i, &seq, productions);
		threads[i] = new std::thread(&Producer::produce, &producers[i]);
	}

	// Join all the threads
	for(int i = 0; i < N; i++) {	
		threads[i]->join();
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
