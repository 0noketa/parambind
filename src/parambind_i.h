/* 2019 10/18 */
#ifndef parambind_i__h
#define parambind_i__h

#include <stdlib.h>
#include <stdint.h>

#include "../include/parambind.h"


/* wrappers for alloc/free, out of this library */
extern void *parambind_i_alloc(size_t);
extern void *parambind_i_free(void*);
extern int parambind_i_init();
extern int parambind_i_clean();

/* internal utils */
extern void parambind_i_bind_func(void *imm, void *func);
extern void parambind_i_bind_func32(void *imm, void *func);
extern void *parambind_i_bind_one(
	size_t template_size, void *template,
	ptrdiff_t func_idx, void *func,
	ptrdiff_t arg_idx, void *arg);
extern void *parambind_i_unbind_func(void *imm);
extern void *parambind_i_unbind_func32(void *imm);
extern void *parambind_i_unbind_one(
	void *code,
	ptrdiff_t func_idx,
	ptrdiff_t arg_idx, void **arg);
extern void *parambind_i_bind_one_abs(
	size_t template_size, void *template,
	ptrdiff_t func_idx, void *func,
	ptrdiff_t arg_idx, void *arg);
extern void *parambind_i_unbind_one_abs(
	void *code,
	ptrdiff_t func_idx,
	ptrdiff_t arg_idx, void **arg);


#endif

