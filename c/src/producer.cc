#include "producer.h"

Producer::Producer() {}
Producer::~Producer() {}

void Producer::act() {
	while(actions > 0) {
		protocol->put(id, id);
		actions--;
	}
}
