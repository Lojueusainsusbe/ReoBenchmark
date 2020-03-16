#include "producer.h"
#include "mutexprint.h"

Producer::Producer(int* d) {
	data = d;
}
Producer::~Producer() {}

void Producer::act() {
	while(actions > 0) {
		protocol->put(id, data);
		actions--;
	}
}
