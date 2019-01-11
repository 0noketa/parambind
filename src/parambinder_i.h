/* .. 2015 5/16 */
/* for implementation */
#ifndef parambind__h
#define parambind__h

#include <stdlib.h>
#include <stdint.h>


/* wrapper for alloc/free, out of this library */
void *parambinder_alloc(int);
void *parambinder_free(void*);

#ifdef FOR_I386_CDECL
#define PARAMBIND_TARGET
#endif

#ifdef FOR_I386_PASCAL
#define PARAMBIND_TARGET
#endif

#ifdef FOR_I386_STDCALL
#define PARAMBIND_TARGET
#endif

#ifndef PARAMBIND_TARGET
#define FOR_I386_CDECL
#endif


/* (a,b->c) -> (b->c) */
void *bind_binary_p1(void *f, void *arg);
void *unbind_binary_p1(void *code, void *f, void *arg);

/* (a,b->c) -> (a->c) */
void *bind_binary_p2(void *f, void *arg);
void *unbind_binary_p2(void *code, void *f, void *arg);

/* (a->b) -> (->b) */
void *pack_anary(void *f, void *arg);
void *unpack_anary(void *code, void *f, void *arg);

/* (a:..., b:...->c) -> (b:...->c) */
void *bind_left(void *f, int closedArgc, void *closedArgv[], int freeArgc);

/* (...->x) -> (->x) */
void *pack(void *f, int argc, void* argv[]);
void *unpack(void *code, int argc, void **f, void* argv[]);
/* only for result of pack() */
int rebind_packed(void *code, int argIdx, void *arg);

/* deallocater for objects from this module */
void *free_binder(void* p);


#endif

