/* closure-like objects for usual cdecl(ia32) or vectorcall(x64) calling convention */
/* 2019 10/18 */
#ifndef parambind__h
#define parambind__h

#include <stdint.h>


extern int parambind_init();
extern int parambind_clean();


#ifdef _WIN64
/* (a,b->c) -> (b->c) */
extern void *parambind_bind_l_vectorcall(void *f, void *arg);
extern void *parambind_unbind_l_vectorcall(void *f, void **out_arg);

/* (a,b->c) -> (a->c) */
extern void *parambind_bind_r_vectorcall(void *f, void *arg);
extern void *parambind_unbind_r_vectorcall(void *f, void **out_arg);

/* (a->b) -> (->b) */
extern void *parambind_bind_u_vectorcall(void *f, void *arg);
extern void *parambind_unbind_u_vectorcall(void *f, void **out_arg);

/* (a:..., b:...->c) -> (b:...->c) */
/*
extern void *parambind_bind_ls_cdecl(void *f, intptr_t argc, intptr_t closedArgc, void *closedArgv[]);
*/

/* (...->x) -> (->x) */
extern void *parambind_bind_a_vectorcall(void *f, intptr_t argc, void *argv[]);
extern void *parambind_unbind_a_vectorcall(void *code, intptr_t argc, void *out_argv[]);


#define parambind_bind_l parambind_bind_l_vectorcall
#define parambind_bind_r parambind_bind_r_vectorcall
#define parambind_bind_u parambind_bind_u_vectorcall
#define parambind_bind_ls parambind_bind_ls_vectorcall
#define parambind_bind_a parambind_bind_a_vectorcall
#define parambind_unbind_l parambind_unbind_l_vectorcall
#define parambind_unbind_r parambind_unbind_r_vectorcall
#define parambind_unbind_u parambind_unbind_u_vectorcall
#define parambind_unbind_a parambind_unbind_a_vectorcall

#else

extern void *parambind_bind_l_cdecl(void *f, void *arg);
extern void *parambind_unbind_l_cdecl(void *f, void **out_arg);
extern void *parambind_bind_r_cdecl(void *f, void *arg);
extern void *parambind_unbind_r_cdecl(void *f, void **out_arg);
extern void *parambind_bind_u_cdecl(void *f, void *arg);
extern void *parambind_unbind_u_cdecl(void *f, void **out_arg);

/*
extern void *parambind_bind_l_stdcall(void *f, void *arg);
extern void *parambind_unbind_l_stdcall(void *f, void **out_arg);
extern void *parambind_bind_r_stdcall(void *f, void *arg);
extern void *parambind_unbind_r_stdcall(void *f, void **out_arg);
extern void *parambind_bind_u_stdcall(void *f, void *arg);
extern void *parambind_unbind_u_stdcall(void *f, void **out_arg);
*/

/* (a:..., b:...->c) -> (b:...->c) */
extern void *parambind_bind_ls_cdecl(void *f, intptr_t argc, intptr_t closedArgc, void *closedArgv[]);

/* (...->x) -> (->x) */
extern void *parambind_bind_a_cdecl(void *f, intptr_t argc, void *argv[]);
extern void *parambind_unbind_a_cdecl(void *code, intptr_t argc, void *out_argv[]);
/*
extern void *parambind_bind_ls_stdcall(void *f, intptr_t argc, intptr_t closedArgc, void *closedArgv[]);
extern void *parambind_bind_a_stdcall(void *f, intptr_t argc, void *argv[]);
extern void *parambind_unbind_a_stdcall(void *code, intptr_t argc, void *out_argv[]);
*/

#define parambind_bind_l parambind_bind_l_cdecl
#define parambind_bind_r parambind_bind_r_cdecl
#define parambind_bind_u parambind_bind_u_cdecl
#define parambind_unbind_l parambind_unbind_l_cdecl
#define parambind_unbind_r parambind_unbind_r_cdecl
#define parambind_unbind_u parambind_unbind_u_cdecl
#define parambind_bind_ls parambind_bind_ls_cdecl
#define parambind_bind_a parambind_bind_a_cdecl
#define parambind_unbind_a parambind_unbind_a_cdecl

#endif


/* deallocater for objects from this module */
extern void *parambind_free(void *f);


#ifdef parambind_opt_aliases
#define bind_l parambind_bind_l
#define bind_r parambind_bind_r
#define bind_u parambind_bind_u
#define bind_a parambind_bind_a
#define bind_ls parambind_bind_ls
#define unbind_l parambind_unbind_l
#define unbind_r parambind_unbind_r
#define unbind_u parambind_unbind_u
#define unbind_a parambind_unbind_a
#endif


#endif
