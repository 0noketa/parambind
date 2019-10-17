/* 2019 10/17 */
/* for implementations */
#ifndef parambind__h
#define parambind__h

#include <stdlib.h>
#include <stdint.h>

/* wrappers for alloc/free, out of this library */
extern void *parambind_i_alloc(size_t);
extern void *parambind_i_free(void*);
extern int parambind_i_init();
extern int parambind_i_clean();

/* internal utils */
extern void parambind_i_bind_func(void *imm, void *func);
extern void *parambind_i_bind_one(
	size_t template_size, void *template,
	uint32_t func_idx, void *func,
	uint32_t arg_idx, void *arg);
extern void *parambind_i_unbind_func(void *imm);
extern void *parambind_i_unbind_one(
	void *code,
	uint32_t func_idx,
	uint32_t arg_idx, void **arg);


/* (a,b->c) -> (b->c) */
void *parambind_bind_l_cdecl(void *f, void *arg);
void *parambind_unbind_l_cdecl(void *code, void **out_arg);
void *parambind_bind_l_stdcall(void *f, void *arg);
void *parambind_unbind_l_stdcall(void *code, void **out_arg);

/* (a,b->c) -> (a->c) */
void *parambind_bind_r_cdecl(void *f, void *arg);
void *parambind_unbind_r_cdecl(void *code, void **out_arg);
void *parambind_bind_r_stdcall(void *f, void *arg);
void *parambind_unbind_r_stdcall(void *code, void **out_arg);

/* (a->b) -> (->b) */
void *parambind_bind_u_cdecl(void *f, void *arg);
void *parambind_unbind_u_cdecl(void *code, void **out_arg);
void *parambind_bind_u_stdcall(void *f, void *arg);
void *parambind_unbind_u_stdcall(void *code, void **out_arg);

/* (a:..., b:...->c) -> (b:...->c) */
void *parambind_bind_ls_cdecl(void *f, intptr_t argc, intptr_t closedArgc, void *closedArgv[]);
void *parambind_bind_ls_stdcall(void *f, intptr_t argc, intptr_t closedArgc, void *closedArgv[]);

/* (...->x) -> (->x) */
void *parambind_bind_a_cdecl(void *f, intptr_t argc, void *argv[]);
void *parambind_unbind_a_cdecl(void *code, intptr_t argc, void *out_argv[]);
void *parambind_bind_a_stdcall(void *f, intptr_t argc, void *argv[]);
void *parambind_unbind_a_stdcall(void *code, intptr_t argc, void *out_argv[]);


/* deallocater for objects from this module */
void *parambind_free(void *p);


#endif

