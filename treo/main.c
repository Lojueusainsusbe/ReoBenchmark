#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reo_rs_ext.h"
#include "assert.h"

#define maxint(a,b) \
  ({int _a = (a), _b = (b); _a > _b ? _a : _b; })

int main(int argc, char** argv) {
  if(argc < 3) {
    printf("Give number of producers and number of consumers\n");
    return -1;
  }

  int p = strtol(argv[1], NULL, 10);
  int c = strtol(argv[2], NULL, 10);

	// create a protocol instance
	CProtoHandle proto = reors_generated_proto_create();

	// claim, initialize the logical ports "A" and "B" and bind them to variables.
	CPutter a[p];
	CGetter b[c];

  char prod[] = "p_";
  char con[] = "x_";
  char res[2+maxint(p,c)];
  char number[maxint(p,c)];

  for(int i = 1; i <= p; i++){
    sprintf(number,"%ld", i);
    strcpy(res, prod);
    strcat(res, number);
    a[i-1] = reors_putter_claim(&proto, res);
  }

  for(int j = 1; j <= c; j++){
    sprintf(number,"%ld", j);
    strcpy(res, con);
    strcat(res, number);
    b[j-1] = reors_getter_claim(&proto, res);
  }

	int value = 42; // data to be sent
	int value2;     // data to be received

	void* from = (void*) &value; // indirection read by reo_rs

	// send the message from A to B py interacting with ports.
	/* put, get, destroy, claim functions are all thread-safe UNLESS they are
	   all performed on the same PORT. eg: `a` and `b` can be on different threads */
	assert(reors_putter_put(&a[0], from)); //TODO 
	void* to = reors_getter_get(&b[0]);
	assert(from == to);

	// Interpret the indirection message received at B
	value2 = *((int *) to);
	assert(value == value2);

	// cleanup. destroy ports and protocol handle (order is irrelevant).
  for(int i = 0; i < p; i++)
    reors_putter_destroy(&a[i]);

  for(int i = 0; i < c; i++)
	 reors_getter_destroy(&b[i]);

	reors_proto_handle_destroy(&proto);

	printf("Everything went fine\n");
	return 0;
}
