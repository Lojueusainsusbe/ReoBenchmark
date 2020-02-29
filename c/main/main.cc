#include "alternator.h"
#include "sequencer.h"
#include "producer.h"
#include "consumer.h"


void SequencerTest(int N, int actions) {
	std::cout << "SEQTEST" << std::endl;
	Producer** producers = (Producer**) malloc(N * sizeof(Producer*));
    Sequence seq(N);

    // Start producing
    pthread_t** threads = (pthread_t**) malloc(N*sizeof(pthread_t*));

    auto tstart = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; i++) {
        threads[i] = new pthread_t;
		producers[i] = new Producer;
        producers[i]->setMembers(i, &seq, actions);
        pthread_create(threads[i], NULL, &Producer::call, producers[i]);
    }

    // Join all the threads
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
		delete producers[i];
    }
    free(threads);
	free(producers);
}


void AlternatorTest(int N, int actions) {
	std::cout << "ALTTEST" << std::endl;
    Producer** producers = (Producer**) malloc(N * sizeof(Producer*));
    Consumer consumer;
    Alternate alt(N);

    // Start producing
    pthread_t** threads = (pthread_t**) malloc(N*sizeof(pthread_t*));

    auto tstart = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; i++) {
        threads[i] = new pthread_t;
		producers[i] = new Producer;
        producers[i]->setMembers(i, &alt, actions);
        pthread_create(threads[i], NULL, &Producer::call, producers[i]);
    }

    // Start consuming
    consumer.setMembers(-1, &alt, actions * N);
    pthread_t conthread;
    pthread_create(&conthread, NULL, &Consumer::call, &consumer);

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
		delete producers[i];
    }
    free(threads);
	free(producers);
}


int main(int argc, char** argv) {
    if(argc < 3) { std::cout << "Give a number of producers and an action count" << std::endl; return -1; }
    int N = strtol(argv[1], NULL, 10);
    int actions = strtol(argv[2], NULL, 10);


	SequencerTest(N, actions);
	AlternatorTest(N, actions);
}
     
