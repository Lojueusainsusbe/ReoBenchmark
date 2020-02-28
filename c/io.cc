#include "io.h"

void IO::setMembers(int i, Protocol* p, int act) {
	id = i;
	protocol = p;
	actions = act;
}

static void* IO::call(void* ptr) {
	(static_cast<IO*>(ptr))->act();
	return NULL;
}
