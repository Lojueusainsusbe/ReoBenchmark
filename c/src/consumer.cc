#include "consumer.h"
#include "mutexprint.h"
#include <iostream>
#include <string>

Consumer::Consumer() {}

Consumer::~Consumer() {}

void Consumer::act() {
	while(actions > 0) {
		int* data = protocol->get(id);
		actions--;
	}
}
