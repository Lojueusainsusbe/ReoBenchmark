#include "consumer.h"

void Consumer::act() override {
	while(actions > 0) {
		int data = protocol->get();
		std::cerr << id << " got " << data << std::endl;
		actions--;
	}
}
