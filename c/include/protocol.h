#ifndef protocol_h
#define protocol_h

class Protocol {
	public:
		Protocol(){}
		// Corresponds to Reo putter
		virtual void put(int id, int d) = 0;

		// Corresponds to Reo getter
		virtual int get() = 0;
};

#endif
