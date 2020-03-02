#ifndef producer_h
#define producer_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C"{
  #include "reo_rs_ext.h"
}

class Producer {
  public:
    Producer();
    ~Producer();
    void setMembers(int a, CProtoHandle* proto, char* name);
    static void* call(void* ptr);

  private:
    void put();

    int actions;
    CPutter putter;
};

#endif
