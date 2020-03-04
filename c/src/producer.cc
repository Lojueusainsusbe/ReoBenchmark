#include "producer.h"
#include "mutexprint.h"

Producer::Producer() {}
Producer::~Producer() {}

void Producer::act() {
	while(actions > 0) {
		mutLog(std::to_string(id) + " putting");
		protocol->put(id, actions);
		actions--;
	}
}
