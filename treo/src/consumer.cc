#include "consumer.h"

Consumer::Consumer(){}

Consumer::~Consumer(){
  reors_getter_destroy(&getter);
}

void Consumer::setMembers(int a, CProtoHandle* proto, char* name){
  actions = a;
  getter = reors_getter_claim(proto, name);
}

void* Consumer::call(void* ptr){
  (static_cast<Consumer*>(ptr))->get();
  return NULL;
}

void Consumer::get(){
  for(int act = 0; act < actions; act++){
    reors_getter_get(&getter);
  }
}
