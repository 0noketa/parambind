/* 2019 10/18 */
/* only for 386+cdecl */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambind_i.h"


#define ARG_IDX 5
#define FUNC_IDX 11
void *parambind_bind_r_cdecl(void *f, void *arg)
{
	const uint8_t template[] = {
		/* pop edx */
		0x5A,
		/* pop eax */
		0x58,
		/* push eax */
		0x50,
		/* push edx */
		0x52,
		/* push imm */
		0x68, 0,0,0,0,
		/* push eax */
		0x50,
		/* call imm(rel) */
		0xE8, 0,0,0,0,
		/* pop edx */
		0x5A,
		/* pop edx */
		0x5A,
		/* ret */
		0xC3
	};

	return parambind_i_bind_one(
			sizeof(template), &template,
			FUNC_IDX, f,
			ARG_IDX, arg);
}
void *parambind_unbind_r_cdecl(void *code, void **arg)
{
	return parambind_i_unbind_one(
		code,
		FUNC_IDX,
		ARG_IDX, arg);
}
#undef ARG_IDX
#undef FUNC_IDX


#define ARG_IDX 2
#define FUNC_IDX 8
void *parambind_bind_r_stdcall(void *f, void *arg)
{
	const uint8_t template[] = {
		/* pop edx */
		0x5A,
		/* push imm */
		0x68, 0,0,0,0,
		/* push edx */
		0x52,
		/* jmp imm(rel) */
		0xE9, 0,0,0,0
	};

	return parambind_i_bind_one(
			sizeof(template), &template,
			FUNC_IDX, f,
			ARG_IDX, arg);
}
void *parambind_unbind_r_stdcall(void *code, void **arg)
{
	return parambind_i_unbind_one(
		code,
		FUNC_IDX,
		ARG_IDX, arg);
}
#undef ARG_IDX
#undef FUNC_IDX


#define ARG_IDX 6
#define FUNC_IDX 11
void *parambind_bind_l_cdecl(void *f, void *arg)
{
	const uint8_t template[] ={
		/* pop edx */
		0x5A,
		/* pop eax */
		0x58,
		/* push eax */
		0x50,
		/* push edx */
		0x52,
		/* push eax */
		0x50,
		/* push imm */
		0x68, 0,0,0,0,
		/* call imm(rel) */
		0xE8, 0,0,0,0,
		/* pop edx */
		0x5A,
		/* pop edx */
		0x5A,
		/* ret */
		0xC3
	};

	return parambind_i_bind_one(
			sizeof(template), &template,
			FUNC_IDX, f,
			ARG_IDX, arg);
}
void *parambind_unbind_l_cdecl(void *code, void **arg)
{
	return parambind_i_unbind_one(
		code,
		FUNC_IDX,
		ARG_IDX, arg);
}
#undef ARG_IDX
#undef FUNC_IDX


#define ARG_IDX 3
#define FUNC_IDX 10
void *parambind_bind_l_stdcall(void *f, void *arg)
{
	const uint8_t template[] = {
		/* pop edx */
		0x5A,
		/* pop eax */
		0x58,
		/* push imm */
		0x68, 0,0,0,0,
		/* push eax */
		0x50,
		/* push edx */
		0x52,
		/* jmp imm(rel) */
		0xE9, 0,0,0,0
	};

	return parambind_i_bind_one(
			sizeof(template), &template,
			FUNC_IDX, f,
			ARG_IDX, arg);
}
void *parambind_unbind_l_stdcall(void *code, void **arg)
{
	return parambind_i_unbind_one(
		code,
		FUNC_IDX,
		ARG_IDX, arg);
}
#undef ARG_IDX
#undef FUNC_IDX


#define ARG_IDX 1
#define FUNC_IDX 6
void *parambind_bind_u_cdecl(void *f, void *arg)
{
	const uint8_t template[] = {
		/* push imm */
		0x68, 0,0,0,0,
		/* call imm(rel) */
		0xE8, 0,0,0,0,
		/* pop edx */
		0x5A,
		/* ret */
		0xC3
	};

	return parambind_i_bind_one(
		sizeof(template), &template,
		FUNC_IDX, f,
		ARG_IDX, arg);
}
void *parambind_unbind_u_cdecl(void *code, void **arg)
{
	return parambind_i_unbind_one(
		code,
		FUNC_IDX,
		ARG_IDX, arg);
}
#undef ARG_IDX
#undef FUNC_IDX

#define ARG_IDX 2
#define FUNC_IDX 8
void *parambind_bind_u_stdcall(void *f, void *arg)
{
	const uint8_t template[] = {
		/* pop edx */
		0x5A,
		/* push imm */
		0x68, 0,0,0,0,
		/* push edx */
		0x52,
		/* jmp imm(rel) */
		0xE9, 0,0,0,0
	};

	return parambind_i_bind_one(
		sizeof(template), &template,
		FUNC_IDX, f,
		ARG_IDX, arg);
}
void *parambind_unbind_u_stdcall(void *code, void **arg)
{
	return parambind_i_unbind_one(
		code,
		FUNC_IDX,
		ARG_IDX, arg);
}
#undef ARG_IDX
#undef FUNC_IDX


void *parambind_free(void* p)
{ 
	return parambind_i_free(p);
}
