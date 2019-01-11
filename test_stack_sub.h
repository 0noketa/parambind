#ifndef test_stack__h

#include <stdint.h>

#define template(t,name) \
typedef struct STRUCT_##name { \
	struct STRUCT_##name *(*del)(struct STRUCT_##name*); \
	void *(*push)(t); \
	void *(*pop)(void); \
	\
	t *stack; \
	t *sp; \
	t *spTail; \
} name;

template(void*,Stack)
template(intptr_t,IntStack)
template(uintptr_t,UIntStack)

#undef template


extern Stack *newStack(int len);

#endif
