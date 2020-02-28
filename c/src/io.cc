#include "io.h"

IO::IO() {}

void IO::setMembers(int i, Protocol* p, int act) {
	id = i;
	protocol = p;
	actions = act;
}

void* IO::call(void* ptr) {
	(static_cast<IO*>(ptr))->act();
	return 0;
}
