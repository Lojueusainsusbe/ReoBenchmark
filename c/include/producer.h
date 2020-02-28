#ifndef producer_h
#define producer_h

#include "io.h"

class Producer : public IO {
	public:
		Producer();
	 	~Producer();
	protected:
    //produce
        void act() override;
};

#endif
