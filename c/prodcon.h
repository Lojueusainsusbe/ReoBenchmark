// #ifdef __PRODCON__
// #define __PRODCON__

// #include "protocol.h"

class Protocol;

class IO {
  public:
    void setMembers(int i, Protocol* p, int act) {
      id = i;
      protocol = p;
      actions = act;
    }

    static void* call(void* ptr) {
      (static_cast<IO*>(ptr))->act();
      return NULL;
    }

  protected:
    //implement producing/consuming actions
    virtual void act() = 0;

    int id;
    int actions;
    Protocol* protocol;
};

class Producer : public IO {
  protected:
    //produce
		void act() override {
			while(actions > 0) {
				protocol->put(id, id);
				actions--;
			}
		}
};

class Consumer : public IO {
  protected:
    //consume
		void act() override {
			while(actions > 0) {
				int data = protocol->get();
        std::cerr << id << " got " << data << std::endl;
				actions--;
			}
		}
};


// #endif
