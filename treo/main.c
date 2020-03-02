#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <chrono>
extern "C"{
  #include "reo_rs_ext.h"
};
#include "assert.h"

#define maxint(a,b) \
  ({int _a = (a), _b = (b); _a > _b ? _a : _b; })

/*
TODO
- schrijf wrappertje:
  vorm ./test alt 3 3 3 3
  alt, ear/rep?, seq
- .h files, makefile etc.
- Timing
- test printen
- kijk of treo filetjes al goed staan
*/

int log10(int num) {
  int count = 1;
  while(num > 1) {
    count++;
    num /= 10;
  }
  return count;
}


class Producer {
  public:
    Producer(){}
    ~Producer(){
      reors_putter_destroy(&putter);
    }
    void setMembers(int a, CProtoHandle* proto, std::string name){
      actions = a;
      putter = reors_putter_claim(proto, const_cast<char*>(name.c_str()));
    }
    static void* call(void* ptr){
      (static_cast<Producer*>(ptr))->put();
    }

  private:
    void put(){
      for(int act = 0; act < actions; act++){
        void* from = (void*) &act;
        reors_putter_put(&putter, from);
      }
    }
    int actions;
    CPutter putter;
};

class Consumer {
  public:
    Consumer(){}
    ~Consumer(){
      reors_getter_destroy(&getter);
    }
    void setMembers(int a, CProtoHandle* proto, std::string name){
      actions = a;
      getter = reors_getter_claim(proto, const_cast<char*>(name.c_str()));
    }
    static void* call(void* ptr){
      (static_cast<Consumer*>(ptr))->get();
    }

  private:
    void get(){
      for(int act = 0; act < actions; act++){
        reors_getter_get(&getter);
      }
    }
    int actions;
    CGetter getter;
};

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
  char res[2+log10(maxint(p,c)+1)+1];
  char number[log10(maxint(p,c)+1)+1];

  for(int i = 0; i < p; i++){
    sprintf(number,"%ld", i+1);
    strcpy(res, prod);
    strcat(res, number);
    prodthreads[i] = new pthread_t;
    producers[i] = new Producer;
    producers[i]->setMembers(pactions, &proto, res);
    pthread_create(prodthreads[i], NULL, &Producer::call, producers[i]);
  }

  for(int i = 0; i < c; i++){
    sprintf(number,"%ld", i+1);
    strcpy(res, con);
    strcat(res, number);
    conthreads[i] = new pthread_t;
    consumers[i] = new Consumer;
    consumers[i]->setMembers(cactions, &proto, res);
    pthread_create(conthreads[i], NULL, &Consumer::call, consumers[i]);
  }

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

  reors_proto_handle_destroy(&proto);

	printf("Everything went fine\n");
	return 0;
}
