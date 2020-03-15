#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>

extern "C"{
  #include "reo_rs_ext.h"
}

#include "producer.h"
#include "consumer.h"


int log10(int num) {
  int count = 1;
  while(num > 1) {
    count++;
    num /= 10;
  }
  return count;
}

int main(int argc, char** argv) {
  if(argc < 5) {
    printf("Give a number of producers, a number of productions, ");
    printf("a number of consumers, and a number of consumptions\n");
    return -1;
  }

  int p = strtol(argv[1], NULL, 10);
  int pactions = strtol(argv[2], NULL, 10);
  int c = strtol(argv[3], NULL, 10);
  int cactions = strtol(argv[4], NULL, 10);

	// create a protocol instance
	CProtoHandle proto = reors_generated_proto_create();

  // initialization of threads
  pthread_t** prodthreads = (pthread_t**) malloc(p * sizeof(pthread_t*));
  pthread_t** conthreads = (pthread_t**) malloc(c * sizeof(pthread_t*));

  // inititialization of the producers and consumers
  Producer** producers = (Producer**) malloc(p * sizeof(Producer*));
  Consumer** consumers = (Consumer**) malloc(c * sizeof(Consumer*));

  // bind names to logical ports and bind ports to threads
  char prod[] = "p_";
  char con[] = "x_";
  char* res = (char*) malloc((2+log10(std::max(p,c)+1)+1) * sizeof(char));
  char* number = (char*) malloc((log10(std::max(p,c)+1)+1) * sizeof(char));

  auto tstart = std::chrono::high_resolution_clock::now();

  // start producing
  for(int i = 0; i < p; i++){
    sprintf(number,"%d", i+1);
    strcpy(res, prod);
    strcat(res, number);
    prodthreads[i] = new pthread_t;
    producers[i] = new Producer;
    producers[i]->setMembers(pactions, &proto, res);
    pthread_create(prodthreads[i], NULL, &Producer::call, producers[i]);
  }

  // start consuming
  for(int i = 0; i < c; i++){
    sprintf(number,"%d", i+1);
    strcpy(res, con);
    strcat(res, number);
    conthreads[i] = new pthread_t;
    consumers[i] = new Consumer;
    consumers[i]->setMembers(cactions, &proto, res);
    pthread_create(conthreads[i], NULL, &Consumer::call, consumers[i]);
  }

  // Timing
	auto tend = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = tend-tstart;
	std::cout << diff.count() << std::endl;

  // cleanup
  for(int i = 0; i < p; i++)
    pthread_join(*prodthreads[i], NULL);

  for(int i = 0; i < c; i++)
    pthread_join(*conthreads[i], NULL);

  for(int i = 0; i < p; i++){
    delete producers[i];
    delete prodthreads[i];
  }

  for(int i = 0; i < c; i++){
    delete consumers[i];
    delete conthreads[i];
  }

  free(producers);
  free(prodthreads);
  free(consumers);
  free(conthreads);
  free(res);
  free(number);

  reors_proto_handle_destroy(&proto);

	printf("Everything went fine\n");
	return 0;
}
