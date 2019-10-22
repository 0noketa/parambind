/* 2019 10/22 */
/* only for 386+cdecl */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambind_i.h"


static const uint8_t
	template_alias[] = {
		/* jmp imm */
		0x69, 0,0,0,0
	},
	template_espLoader[] = {
		/* mov eax, esp */
		0x89, 0xE0
	},
	template_openedArg[] = {
		/* push [eax+imm8] */
		0xFF, 0x70, 0
	},
	template_closedArg[] = {
		/* push imm */
		0x68, 0,0,0,0
	},
	template_body_cdecl[] = {
		/* call imm(rel) */
		0xE8, 0,0,0,0,
		/* add esp, imm32 */
		0x81, 0xC4, 0,0,0,0,
		/* ret */
		0xC3
	},
	template_body_stdcall[] = {
		/* call imm(rel) */
		0xE8, 0,0,0,0,
		/* pop edx */
		0x5A,
		/* add esp, imm32 */
		0x81, 0xC4, 0,0,0,0,
		/* push edx */
		0x52,
		/* ret */
		0xC3
	};

static size_t write_espLoader(void *dst)
{
	memcpy(dst, template_espLoader, sizeof(template_espLoader));

	return sizeof(template_espLoader);
}

static size_t write_openedArg_cdecl(void *dst, int argc, int8_t idx)
{
	memcpy(dst, template_openedArg, sizeof(template_openedArg));

	int8_t *p = (int8_t*)dst + 2;
	*p = (idx + 1) * 4;

	return sizeof(template_openedArg);
}

static size_t write_openedArg_stdcall(void *dst, int argc, int8_t idx)
{
	memcpy(dst, template_openedArg, sizeof(template_openedArg));

	int8_t *p = (int8_t*)dst + 2;
	*p = (argc - idx) * 4;

	return sizeof(template_openedArg);
}

static size_t write_closedArg(void *dst, void *arg)
{
	memcpy(dst, template_closedArg, sizeof(template_closedArg));

	void **p = (void*)((uint8_t*)dst + 1);
	*p = arg;

	return sizeof(template_closedArg);
}

static size_t write_body_cdecl(void *dst, void *func, int closedArgc)
{
	memcpy(dst, template_body_cdecl, sizeof(template_body_cdecl));

	parambind_i_bind_func((uint8_t*)dst + 1, func);

	void **p = (void*)((uint8_t*)dst + 7);
	*p = (void*)(uintptr_t)(closedArgc * 4);

	return sizeof(template_body_cdecl);
}

static size_t write_body_stdcall(void *dst, void *func, int closedArgc)
{
	memcpy(dst, template_body_stdcall, sizeof(template_body_stdcall));

	parambind_i_bind_func((uint8_t*)dst + 1, func);

	uint8_t *p = (uint8_t*)dst + 8;
	*p = closedArgc * 4;

	return sizeof(template_body_stdcall);
}

/*
 intptr_t f(intptr_t a, intptr_t b, intptr_t c, intptr_t d, intptr_t e);
 intptr_t subargs[] = { 1, 2 };
 intptr_t (*f2)(intptr_t, intptr_t) = parambind_bind_ls(f, 5, subargs, 2);
 f2(3,4,5);  //equals to f(1,2,3,4,5)
*/
void *parambind_bind_ls_cdecl(void *f, intptr_t argc, intptr_t closedArgc, void *closedArgv[])
{
	intptr_t openedArgc = argc - closedArgc;
	size_t size = sizeof(template_espLoader)
			+ sizeof(template_closedArg) * closedArgc 
			+ sizeof(template_openedArg) * openedArgc 
			+ sizeof(template_body_cdecl);

	if (argc < closedArgc || argc < 0 || argc > 28) return NULL;
	
	uint8_t *code = parambind_i_alloc(size);
	
	if (code)
	{
		uint8_t *p = code;

		p += write_espLoader(p);

		for(intptr_t i = openedArgc; i-- > 0;)
		{
			p += write_openedArg_cdecl(p, argc, closedArgc + i);
		}

		for(intptr_t i = 0; i < closedArgc; ++i)
		{
			p += write_closedArg(p, closedArgv[closedArgc - i - 1]);
		}

		write_body_cdecl(p, f, closedArgc);
	}	

	return code;
}


void *parambind_bind_ls_stdcall(void *f, intptr_t argc, intptr_t closedArgc, void *closedArgv[])
{
	intptr_t openedArgc = argc - closedArgc;
	size_t size = sizeof(template_espLoader)
			+ sizeof(template_closedArg) * closedArgc 
			+ sizeof(template_openedArg) * openedArgc 
			+ sizeof(template_body_stdcall);

	if (argc < closedArgc || argc < 0 || argc > 28) return NULL;
	
	uint8_t *code = parambind_i_alloc(size);
	
	if (code)
	{
		uint8_t *p = code;

		p += write_espLoader(p);

		for(intptr_t i = 0; i < closedArgc; ++i)
		{
			p += write_closedArg(p, closedArgv[i]);
		}

		for(intptr_t i = 0; i < openedArgc; ++i)
		{
			p += write_openedArg_stdcall(p, argc, i);
		}

		write_body_stdcall(p, f, closedArgc);
	}	

	return code;
}


void *parambind_bind_a_cdecl(void *f, intptr_t argc, void *argv[])
{
	size_t size = sizeof(template_espLoader)
		+ sizeof(template_closedArg) * argc 
		+ sizeof(template_body_cdecl);

	if (argc < 0 || argc > 256) return NULL;

	uint8_t *code = parambind_i_alloc(size);
	
	if (code)
	{
		intptr_t i;
		uint8_t *p = code;

		for(intptr_t i = argc; i-- > 0;)
		{
			p += write_closedArg(p, argv[i]);
		}

		write_body_cdecl(p, f, argc);
	}	

	return code;
}

void *parambind_unbind_a_cdecl(void *code, intptr_t argc, void *argv[])
{
	size_t size = sizeof(template_closedArg) * argc + sizeof(template_body_cdecl);

	if (code)
	{
		uint8_t *p = code;

		for (intptr_t i = 0; i < argc; ++i)
		{
			argv[argc - i - 1] = *(void**)(p + 1);
			p += sizeof(template_closedArg);
		}

		void *func = parambind_i_unbind_func(p + 1);

		parambind_i_free(code);

		return func;
	}	
	else
		return 0;
}


void *parambind_bind_a_stdcall(void *f, intptr_t argc, void *argv[])
{
	size_t size = sizeof(template_espLoader)
		+ sizeof(template_closedArg) * argc 
		+ sizeof(template_body_stdcall);

	if (argc < 0 || argc > 256) return NULL;

	uint8_t *code = parambind_i_alloc(size);
	
	if (code)
	{
		intptr_t i;
		uint8_t *p = code;

		for(intptr_t i = 0; i < argc; ++i)
		{
			p += write_closedArg(p, argv[i]);
		}

		write_body_stdcall(p, f, argc);
	}	

	return code;
}

void *parambind_unbind_a_stdcall(void *code, intptr_t argc, void *argv[])
{
	size_t size = sizeof(template_closedArg) * argc + sizeof(template_body_cdecl);

	if (code)
	{
		uint8_t *p = code;

		for (intptr_t i = 0; i < argc; ++i)
		{
			argv[i] = *(void**)(p + 1);
			p += sizeof(template_closedArg);
		}

		void *func = parambind_i_unbind_func(p + 1);

		parambind_i_free(code);

		return func;
	}	
	else
		return 0;
}
