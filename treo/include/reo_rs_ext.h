// reo_rs c interface header
// source: https://github.com/sirkibsirkib/reo_rs

#ifndef REORS_DEF
#define REORS_DEF

#include <stdint.h>
#include <stdbool.h>

typedef struct CProtoHandle {
  uintptr_t inner;
} CProtoHandle;

CProtoHandle reors_empty_proto_create();
void reors_proto_handle_destroy(CProtoHandle*);

typedef struct CPutter {
  uintptr_t locid;
  uintptr_t tid;
  uintptr_t handle;
} CPutter;

CPutter reors_putter_claim(CProtoHandle*, char*);
bool reors_putter_put(CPutter*, void*);
void reors_putter_destroy(CPutter*);

typedef struct CGetter {
  uintptr_t locid;
  uintptr_t tid;
  uintptr_t handle;
} CGetter;

CGetter reors_getter_claim(CProtoHandle*, char*);
void* reors_getter_get(CGetter*);
void reors_getter_destroy(CGetter*);

/////////////////////

CProtoHandle reors_generated_proto_create();

#endif
