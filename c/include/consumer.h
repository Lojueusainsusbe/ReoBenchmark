#ifndef consumer_h
#define consumer_h

#include "io.h"

class Consumer : public IO {
	public:
		Consumer();
		~Consumer();
	protected:
    	//consume
        void act() override;
};

#endif
