// #ifdef __PROTOCOL__
// #define __PROTOCOL__

class Protocol {
	public:
		// Corresponds to Reo putter
		virtual void put(int id, int d) = 0;

		// Corresponds to Reo getter
		virtual int get() = 0;
};

// #endif
