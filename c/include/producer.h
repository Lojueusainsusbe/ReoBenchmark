#ifndef producer_h
#define producer_h

#include "io.h"

class Producer : public IO {
  protected:
    //produce
        void act() override;
};

#endif
