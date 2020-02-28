#include "protocol.h"

#ifndef IO_h
#define IO_h

class IO {
	public:
		IO();
		virtual ~IO() {}
		void setMembers(int i, Protocol* p, int act);
	  	static void* call(void* ptr);

	protected:
		//implement producing/consuming actions
		virtual void act() = 0;

		int id;
		int actions;
		Protocol* protocol;
};

#endif

