#ifndef producer_h
#define producer_h

#include "io.h"

class Producer : public IO {
	public:
		Producer(int* d);
	 	~Producer();
	protected:
    //produce
        void act() override;
	private:
		int* data;
};

#endif
