//c++ code for krepeater

#include<stdio.h>
#include<mutex>
#include<iostream>
#include<chrono>
#include<thread>

class Repeater{
	public:
		Repeater(int num) {
			k = num;
			count = 0;
			p.unlock();
			g.lock();
		}

		void put(int data) {
			p.lock();
			buff = data;
			count = k;
			g.unlock();
		}

		int get() {
			g.lock();
			count--;
			if(count == 0)
				p.unlock();
			else
				g.unlock();
			return buff;
		}

	private:
		std::mutex p;
		std::mutex g;
		int k;
		int buff;
		int count;
};

class Producer {
	public:
		Producer() {}
		void setMembers(Repeater* p){
			repeater = p;
		}

		void produce() {
			int data = 0;
			std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
			while(true) {
				repeater->put(data);
				data++;
				std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
			}
		}
	private:
		Repeater* repeater;
};

class Consumer {
	public:
		Consumer() {}
		void setMembers(Repeater *p){
			repeater = p;
		}

		void consume(){
			while(true){
				int data = repeater->get();
				std::cout << data << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(20*(rand()%50)));
			}
		}
	private:
		Repeater* repeater;
};

int main(int argc, char** argv){
	if(argc < 2) { std::cout << "Give k" << std::endl; return -1; }
  int k = strtol(argv[1], NULL, 10);

	Producer producer;
	Consumer consumer;
	Repeater repeater(k);

	producer.setMembers(&repeater);
	consumer.setMembers(&repeater);

	std::thread* p = new std::thread(&Producer::produce, &producer);
	std::thread* c = new std::thread(&Consumer::consume, &consumer);

	p->join();
	c->join();

  return 0;
}
