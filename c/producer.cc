#include "producer.h"

void Producer::act() override {
	while(actions > 0) {
		protocol->put(id, id);
		actions--;
	}
}
