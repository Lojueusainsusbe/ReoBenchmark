#include "alternator.h"
#include "sequencer.h"
#include "eaoseq.h"
#include "earep.h"
#include "producer.h"
#include "consumer.h"


Protocol* protocolFromString(std::string s, int Nprod, int Ncons) {
	if(s == "alt") {
		std::cerr << "Testing Alternator with " << Nprod << " producers." << std::endl;
		return new Alternate(Nprod);
	}
	else if(s == "seq") {
		std::cerr << "Testing Sequencer with " << Nprod << " producers." << std::endl;
		return new Sequence(Nprod);
	}
	else if(s == "ear") {
		std::cerr << "Testing EARep with " << Ncons << " consumers." << std::endl;
		return new EARep(Ncons);
	}
	else if(s == "eao") {
		std::cerr << "Testing EAOSequencer with " << Ncons << " consumers." << std::endl;
		return new EAOSequence(Ncons);
	}
	else {
		std::cerr << "Invalid protocol name." << std::endl;
		return NULL;
	}
}


void test(Protocol* p, int Nprod, int actProd, int Ncons, int actCons) {
	int data = 1;	
	
	Producer** producers = (Producer**) malloc(Nprod * sizeof(Producer*));
	Consumer** consumers = (Consumer**) malloc(Ncons * sizeof(Consumer*));

	pthread_t** pthreads = (pthread_t**) malloc(Nprod*sizeof(pthread_t*));
	pthread_t** cthreads = (pthread_t**) malloc(Ncons*sizeof(pthread_t*));

	auto tstart = std::chrono::high_resolution_clock::now();

	// Start producing
	for(int i = 0; i < Nprod; i++) {
	    pthreads[i] = new pthread_t;
		producers[i] = new Producer(&data);
		producers[i]->setMembers(i, p, actProd);
	    pthread_create(pthreads[i], NULL, &Producer::call, producers[i]);
	}

	// Start consuming
	for(int i = 0; i < Ncons; i++) {
		cthreads[i] = new pthread_t;
		consumers[i] = new Consumer;
		consumers[i]->setMembers(i, p, actCons);
		pthread_create(cthreads[i], NULL, &Consumer::call, consumers[i]);
	}

	// Join all the threads
	for(int i = 0; i < Nprod; i++) {
		pthread_join(*pthreads[i], NULL);
	}
	for(int i = 0; i < Ncons; i++) {
	    pthread_join(*cthreads[i], NULL);
	}

	// Timing
	auto tend = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = tend-tstart;
	std::cout << diff.count() << std::endl;

	// Cleanup
	for(int i = 0; i < Nprod; i++) {
		delete pthreads[i];
		delete producers[i];
	}
	for(int i = 0; i < Ncons; i++) {
		delete cthreads[i];
		delete consumers[i];
	}
	free(pthreads);
	free(cthreads);
	free(producers);
	free(consumers);
}


int main(int argc, char** argv) {
  if(argc < 6) {
		std::cerr 	<< "Give a protocol, "
		 			<< "number of producers, "
					<< "a number of productions, "
					<< "a number of consumers, "
					<< "and a number of consumptions" << std::endl;
		return -1;
	}

 	int Nprod = strtol(argv[2], NULL, 10);
	int actProd = strtol(argv[3], NULL, 10);
	int Ncons = strtol(argv[4], NULL, 10);
	int actCons = strtol(argv[5], NULL, 10);
	Protocol* p = protocolFromString(std::string(argv[1]), Nprod, Ncons);

	//Run test
	if(p != NULL)
		test(p, Nprod, actProd, Ncons, actCons);

	delete p;
}
