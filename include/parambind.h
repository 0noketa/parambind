/* closure-like objects for usual cdecl(ia32) or fastcall64(x64) calling convention */
/* 2019 10/23 */
#ifndef parambind__h
#define parambind__h

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


extern int parambind_init(void);
extern int parambind_clean(void);


#ifdef _WIN64
/* (a,b->c) -> (b->c) */
extern void *parambind_bind_l_fastcall64(void *f, void *arg);
extern void *parambind_unbind_l_fastcall64(void *f, void **out_arg);

/* (a,b->c) -> (a->c) */
extern void *parambind_bind_r_fastcall64(void *f, void *arg);
extern void *parambind_unbind_r_fastcall64(void *f, void **out_arg);

/* (a->b) -> (->b) */
extern void *parambind_bind_u_fastcall64(void *f, void *arg);
extern void *parambind_unbind_u_fastcall64(void *f, void **out_arg);

/* (a:..., b:...->c) -> (b:...->c) */
/* 0 <= argc <= 14 */
extern void *parambind_bind_ls_fastcall64(
    void *f, intptr_t argc,
    intptr_t closedArgc, void *closedArgv[]);

/* (a:..., b:...->c) -> (a:...->c) */
/* 0 <= argc <= 6 */
extern void *parambind_bind_rs_fastcall64(
    void *f, intptr_t argc,
    intptr_t closedArgc, void *closedArgv[]);

/* (...->x) -> (->x) */
/* 0 <= argc <= 14 */
extern void *parambind_bind_a_fastcall64(void *f, intptr_t argc, void *argv[]);
extern void *parambind_unbind_a_fastcall64(void *code, intptr_t argc, void *out_argv[]);


extern void *parambind_bind_l_amd64(void *f, void *arg);
extern void *parambind_bind_r_amd64(void *f, void *arg);
extern void *parambind_bind_u_amd64(void *f, void *arg);
/* 0 <= argc <= 14 */
extern void *parambind_bind_a_amd64(void *f, intptr_t argc, void *argv[]);

extern void *parambind_unbind_l_amd64(void *f, void **out_arg);
extern void *parambind_unbind_r_amd64(void *f, void **out_arg);
extern void *parambind_unbind_u_amd64(void *f, void **out_arg);

extern void *parambind_unbind_a_amd64(void *code, intptr_t argc, void *out_argv[]);

/* 0 <= argc <= 14 */
extern void *parambind_bind_ls_amd64(
    void *f, intptr_t argc,
    intptr_t closedArgc, void *closedArgv[]);
/* 0 <= argc <= 6 */
extern void *parambind_bind_rs_amd64(
    void *f, intptr_t argc,
    intptr_t closedArgc, void *closedArgv[]);


#define parambind_bind_l parambind_bind_l_fastcall64
#define parambind_bind_r parambind_bind_r_fastcall64
#define parambind_bind_u parambind_bind_u_fastcall64
#define parambind_bind_ls parambind_bind_ls_fastcall64
#define parambind_bind_rs parambind_bind_rs_fastcall64
#define parambind_bind_a parambind_bind_a_fastcall64
#define parambind_unbind_l parambind_unbind_l_fastcall64
#define parambind_unbind_r parambind_unbind_r_fastcall64
#define parambind_unbind_u parambind_unbind_u_fastcall64
#define parambind_unbind_a parambind_unbind_a_fastcall64


/* amd64 -> fastcall64 */
/* argc <= 4 */
extern void *parambind_wrapas_fastcall64_amd64(void *f, intptr_t argc);
/* fastcall64 -> amd64 */
/* argc <= 4 */
extern void *parambind_wrapas_amd64_fastcall64(void *f, intptr_t argc);

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
#define bind_rs parambind_bind_rs
#define unbind_l parambind_unbind_l
#define unbind_r parambind_unbind_r
#define unbind_u parambind_unbind_u
#define unbind_a parambind_unbind_a
#endif


#ifdef __cplusplus
}
#endif


#endif
