#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>

class XConsumer {
	public:
		XConsumer() {}
    void init(int i, int* b, std::mutex* wl, std::mutex* rl) {
      id = i;
      buff = b;
      write_lock = wl;
      read_lock = rl;
    }

		void consume() {
      while(true) {
  			read_lock->lock();

  			std::cout << id << " reads: " << *buff << std::endl;

  			write_lock->unlock(); //producer may now overwrite value

        std::this_thread::sleep_for(std::chrono::milliseconds((id+1)*20*(rand()%50)));
      }
		}

	private:
    int id;
    int* buff;
		std::mutex* write_lock;
    std::mutex* read_lock;
};


class Producer {
	public:
		Producer() {}
		void init(int* b, std::mutex* wl, std::mutex* rl) {
      buff = b;
      write_lock = wl;
      read_lock = rl;
		}

		void produce() {
			// Randomize when consumers start
			std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
			while(true) {
        write_lock->lock();

				*buff = rand();

        read_lock->unlock(); //buffer is free for reading

        std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
			}
		}
	private:
    int* buff;
		std::mutex* write_lock;
    std::mutex* read_lock;
};


int main(int argc, char** argv) {
	if(argc < 2) { std::cerr << "Give a number of Consumers" << std::endl; return -1; }
	int N = strtol(argv[1], NULL, 10);

	Producer producer;
	XConsumer consumers[N];

	// Start producing
	std::thread** threads = (std::thread**) malloc(N*sizeof(std::thread*));

  int shared_buffer;
  std::mutex shared_write_lock;
  std::mutex shared_read_lock;
  shared_read_lock.lock();

  producer.init(&shared_buffer, &shared_write_lock, &shared_read_lock);

	for(int i = 0; i < N; i++) {
    consumers[i].init(i, &shared_buffer, &shared_write_lock, &shared_read_lock);
		threads[i] = new std::thread(&XConsumer::consume, &consumers[i]);
  }

  producer.produce();

	// Join all the threads
	for(int i = 0; i < N; i++) {
		threads[i]->join();
	}

}
