#ifndef test_stack__h

#include <stdint.h>

#define template(t,name) \
typedef struct STRUCT_##name { \
	struct STRUCT_##name *(*del)(struct STRUCT_##name*); \
	intptr_t (*push)(t); \
	t (*pop)(void); \
	intptr_t (*len)(void); \
	\
	t *stack; \
	t *sp; \
	t *spTail; \
} name;

template(void*,Stack)
template(intptr_t,IntStack)
template(uintptr_t,UIntStack)

#undef template


extern Stack *newStack(uintptr_t len);
#define newIntStack(len) ((IntStack*)newStack((len)))
#define newUIntStack(len) ((UIntStack*)newStack((len)))

#endif
