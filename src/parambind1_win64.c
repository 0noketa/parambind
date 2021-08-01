/* 2019 10/18 */
/* only for x64+fastcall64 */
#ifdef _WIN64
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambind_i.h"


#define ARG_IDX 23
#define FUNC_IDX 6
void *parambind_bind_l_fastcall64(void *f, void *arg)
{
	const uint8_t template[] = {
		/* sub rsp, imm8(24) */
		0x48, 0x83, 0xEC, 24,
		/* mov rdx, imm */
		0x48, 0xBA, 0,0,0,0, 0,0,0,0,
		/* mov [rsp], rdx */
		0x48, 0x89, 0x14, 0x24,
		/* mov rdx, rcx */
		0x48, 0x89, 0xCA,
		/* mov rcx, imm */
		0x48, 0xB9, 0,0,0,0, 0,0,0,0,
		/* call [rsp] */
		0xFF, 0x14,0x24,
		/* add rsp, imm8(24) */
		0x48, 0x83, 0xC4, 24,
		/* ret */
		0xC3
	};

	return parambind_i_bind_one_abs(
			sizeof(template), &template,
			FUNC_IDX, f,
			ARG_IDX, arg);
}
void *parambind_unbind_l_fastcall64(void *code, void **arg)
{
	return parambind_i_unbind_one_abs(
		code,
		FUNC_IDX,
		ARG_IDX, arg);
}
#undef ARG_IDX
#undef FUNC_IDX


#define ARG_IDX 20
#define FUNC_IDX 6
void *parambind_bind_r_fastcall64(void *f, void *arg)
{
	const uint8_t template[] = {
		/* sub rsp, imm8(24) */
		0x48, 0x83, 0xEC, 24,
		/* mov rdx, imm */
		0x48, 0xBA, 0,0,0,0, 0,0,0,0,
		/* mov [rsp], rdx */
		0x48, 0x89, 0x14, 0x24,
		/* mov rdx, imm */
		0x48, 0xBA, 0,0,0,0, 0,0,0,0,
		/* call [rsp] */
		0xFF, 0x14,0x24,
		/* add rsp, imm8(24) */
		0x48, 0x83, 0xC4, 24,
		/* ret */
		0xC3
	};

	return parambind_i_bind_one_abs(
			sizeof(template), &template,
			FUNC_IDX, f,
			ARG_IDX, arg);
}
void *parambind_unbind_r_fastcall64(void *code, void **arg)
{
	return parambind_i_unbind_one_abs(
		code,
		FUNC_IDX,
		ARG_IDX, arg);
}
#undef ARG_IDX
#undef FUNC_IDX


void *parambind_bind_u_fastcall64(void *f, void *arg)
{
	return parambind_bind_l_fastcall64(f, arg);
}
void *parambind_unbind_u_fastcall64(void *code, void **arg)
{
	return parambind_unbind_l_fastcall64(code, arg);
}


void *parambind_free(void* p)
{ 
	return parambind_i_free(p);
}


#endif
