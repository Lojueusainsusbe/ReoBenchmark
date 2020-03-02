#ifndef consumer_h
#define consumer_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C"{
  #include "reo_rs_ext.h"
}

class Consumer {
  public:
    Consumer();
    ~Consumer();
    void setMembers(int a, CProtoHandle* proto, char* name);
    static void* call(void* ptr);

  private:
    void get();

    int actions;
    CGetter getter;
};

#endif
