#include "producer.h"

Producer::Producer(){}

Producer::~Producer(){
  reors_putter_destroy(&putter);
}

void Producer::setMembers(int a, CProtoHandle* proto, char* name){
  actions = a;
  putter = reors_putter_claim(proto, name);
}

void* Producer::call(void* ptr){
  (static_cast<Producer*>(ptr))->put();
  return NULL;
}

void Producer::put(){
  int* data;
  for(int act = 0; act < actions; act++){
    data = new int(act);
    void* from = (void*) data;
    reors_putter_put(&putter, from);
  }
}
