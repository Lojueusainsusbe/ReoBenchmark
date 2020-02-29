#include <mutex>
#include <iostream>

std::mutex logmutex;

void mutLog(std::string msg) {
	logmutex.lock();
	std::cout << msg << std::endl;
	logmutex.unlock();
}
