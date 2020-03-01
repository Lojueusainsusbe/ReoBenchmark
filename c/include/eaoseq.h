/*
  EarlyAsyncOutSequencer
  Single producer, k consumer
  Produces a data item that is consumed by one of the consumers
  Consumers take turns in defined sequence
*/

#ifndef eaoseq_h
#define eaoseq_h

#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>
#include<string>

#include "protocol.h"

class EAOSequence : public Protocol {

  public:
    EAOSequence(int num);
    ~EAOSequence();

  // Producer id does a put on the protocol
  void put(int id, int data) override;

  // This protocol does not have outputs
  int get(int id) override;
  private:
    pthread_mutex_t* c_locks;
    pthread_mutex_t buf_lock;
    int N;
    int turn;
    int buff;
};

#endif
