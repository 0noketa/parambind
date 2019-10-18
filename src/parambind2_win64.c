/* 2019 10/18 */
/* only for x64+vectorcall */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambind_i.h"


static const uint8_t
	template_alias[] = {
		/* jmp imm */
		0x69, 0,0,0,0
	},
	template_allocAuto[] = {
		/* sub rsp, imm8 */
		0x48, 0x83, 0xEC, 0
	},
	template_funcLoader[] = {
		/* mov rdx, imm */
		0x48, 0xBA, 0,0,0,0, 0,0,0,0,
		/* mov [rsp], rdx */
		0x48, 0x89, 0x14, 0x24
	},
	template_openedArg[] = {
		/* push [eax+imm8] */
		0xFF, 0x70, 0
	},
	template_movRcx[] = {
		/* mov rcx, imm */
		0x48, 0xB9, 0,0,0,0, 0,0,0,0
	},
	template_movRdx[] = {
		/* mov rdx, imm */
		0x48, 0xBA, 0,0,0,0, 0,0,0,0
	},
	template_movR8[] = {
		/* mov r8, imm */
		0x49, 0xB8, 0,0,0,0, 0,0,0,0
	},
	template_movR9[] = {
		/* mov r9, imm */
		0x49, 0xB9, 0,0,0,0, 0,0,0,0
	},
	template_closedArg[] = {
		/* mov rdx, imm */
		0x48, 0xBA, 0,0,0,0, 0,0,0,0,
		/* mov [rsp+imm], rdx */
		0x48, 0x89, 0x54, 0x24, 0
	},
	template_peekAndCall[] = {
		/* call [rsp] */
		0xFF, 0x14, 0x24
	},
	template_deallocAuto[] = {
		/* add rsp, imm8 */
		0x48, 0x83, 0xC4, 0
	},
	template_ret[] = {
		0xC3
	};

static size_t write_allocAuto(void *dst, uint8_t size)
{
	memcpy(dst, template_allocAuto, sizeof(template_allocAuto));

	uint8_t *p = (uint8_t*)dst + 3;
	*p = size;

	return sizeof(template_allocAuto);
}
static size_t write_deallocAuto(void *dst, uint8_t size)
{
	memcpy(dst, template_deallocAuto, sizeof(template_deallocAuto));

	uint8_t *p = (uint8_t*)dst + 3;
	*p = size;

	return sizeof(template_deallocAuto);
}
static size_t write_funcLoader(void *dst, void *f)
{
	memcpy(dst, template_funcLoader, sizeof(template_funcLoader));

	void **p = (void*)((uint8_t*)dst + 2);
	*p = f;

	return sizeof(template_funcLoader);
}

static size_t write_closedArg(void *dst, int idx, void *arg)
{
	if (idx < 4)
	{
		memcpy(dst,
			(idx == 0 ? template_movRcx
			: idx == 1 ? template_movRdx
			: idx == 2 ? template_movR8
			: template_movR9),
			sizeof(template_movRcx));

		void **p = (void*)((uint8_t*)dst + 2);
		*p = arg;

		return sizeof(template_movRcx);
	}
	else
	{
		memcpy(dst, template_closedArg, sizeof(template_closedArg));

		uint8_t *d = (void*)((uint8_t*)dst + 14);
		*d = idx * 8;

		void **p = (void*)((uint8_t*)dst + 2);
		*p = arg;

		return sizeof(template_closedArg);
	}	
}
static size_t read_closedArg(void *dst, int idx, void **arg)
{
	if (idx < 4)
	{
		void **p = (void*)((uint8_t*)dst + 2);
		*arg = *p;

		return sizeof(template_movRcx);
	}
	else
	{
		void **p = (void*)((uint8_t*)dst + 2);
		*arg = *p;

		return sizeof(template_closedArg);
	}	
}

static size_t write_peekAndCall(void *dst)
{
	memcpy(dst, template_peekAndCall, sizeof(template_peekAndCall));

	return sizeof(template_peekAndCall);
}
static size_t write_ret(void *dst)
{
	memcpy(dst, template_ret, sizeof(template_ret));

	return sizeof(template_ret);
}


/*
 intptr_t f(intptr_t a, intptr_t b, intptr_t c, intptr_t d, intptr_t e);
 intptr_t subargs[] = { 1, 2 };
 intptr_t (*f2)(intptr_t, intptr_t) = parambind_bind_ls(f, 5, subargs, 2);
 f2(3,4,5);  //equals to f(1,2,3,4,5)
*/
void *parambind_bind_ls_cdecl(void *f, intptr_t argc, intptr_t closedArgc, void *closedArgv[])
{
	return NULL;
}



void *parambind_bind_a_vectorcall(void *f, intptr_t argc, void *argv[])
{
	if (argc < 0 || argc > 30) return NULL;

	size_t size = sizeof(template_allocAuto)
		+ sizeof(template_funcLoader)
		+ (argc > 4
			? sizeof(template_movRcx) * 4
				+ sizeof(template_closedArg) * (argc - 4) 
			: sizeof(template_movRcx) * argc)
		+ sizeof(template_peekAndCall)
		+ sizeof(template_deallocAuto)
		+ sizeof(template_ret);

	uint8_t vaSize = argc * 8;

	if (vaSize % 16 == 0) vaSize += 8;
	if (vaSize <= 32) vaSize = 40;

	uint8_t *code = parambind_i_alloc(size);
	
	if (code)
	{
		uint8_t *p = code;

		p += write_allocAuto(p, vaSize);
		p += write_funcLoader(p, f);

		for(intptr_t i = 0; i < argc; ++i)
		{
			p += write_closedArg(p, i, argv[i]);
		}

		p += write_peekAndCall(p);
		p += write_deallocAuto(p, vaSize);
		p += write_ret(p);
	}	

	return code;
}

void *parambind_unbind_a_vectorcall(void *code, intptr_t argc, void *argv[])
{
	if (code == NULL
		|| argc < 0 || argc > 30 
		|| argc > 0 && argv == NULL)
		return NULL;

	uint8_t *p = code;
	
	p += sizeof(template_allocAuto);

	void *func = *(void**)(void*)(p + 2);

	p += sizeof(template_funcLoader);

	for (int i = 0; i < argc; ++i)
	{
		p += read_closedArg(p, i, &argv[i]);
	}

	return func;
}

