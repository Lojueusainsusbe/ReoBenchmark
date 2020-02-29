#include "consumer.h"
#include <iostream>

Consumer::Consumer() {}

Consumer::~Consumer() {}

void Consumer::act() {
	while(actions > 0) {
		int data = protocol->get();
		actions--;
	}
}
