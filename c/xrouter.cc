#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>


class xRoute {
	public:
		xRoute(int n) {
			N = n;
		}
		~xRoute() {
		
		}

		void put(int id, int data) {
		
		}

		int get() {
			
		}

	private:
		int N;
};


class Producer {
    public:
        Producer() {}
        void setMembers(xRoute* x, int act) {
            xr = x;
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
        int actions;
        xRoute* xr;
};


class Consumer {
    public:
        Consumer() {}
        void setMembers(xRoute* x, int act) {
            xr = x;
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
        xRoute* alt;
};


int main(int argc, char** argv) {
    if(argc < 3) { std::cout << "Give a number of producers and an action count" << std::endl; return -1; }
    int N = strtol(argv[1], NULL, 10);
    int productions = strtol(argv[2], NULL, 10);

    Producer consumers[N];
    Consumer producer;
   	xRoute xR(N);

    // Start producing
    pthread_t** threads = (pthread_t**) malloc(N*sizeof(pthread_t*));

    auto tstart = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; i++) {
        threads[i] = new pthread_t;
        consumers[i].setMembers(&xR, productions);
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

