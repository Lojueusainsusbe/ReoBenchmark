#include "producer.h"
#include "mutexprint.h"

Producer::Producer() {}
Producer::~Producer() {}

void Producer::act() {
	while(actions > 0) {
		protocol->put(id, actions);
		actions--;
	}
}
