/* 2019/10/18 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambind_i.h"


int parambind_init()
{
	return (
#ifdef _WIN64
			sizeof(void*) == 8
			&& sizeof(void (*)()) == 8
			&& sizeof(void (* (*)() )()) == 8
#else
			sizeof(void*) == 4
			&& sizeof(void (*)()) == 4
			&& sizeof(void (* (*)() )()) == 4
#endif
		)
		&& parambind_i_init();
}
int parambind_clean()
{
	return parambind_i_clean();
}


void parambind_i_bind_func_d(void *imm, void *func) 
{
	intptr_t *p = imm;
	ptrdiff_t d = (uint8_t*)func - ((uint8_t*)imm + sizeof(void*));
	*p = d;
}
void *parambind_i_unbind_func_d(void *imm) 
{
	intptr_t *p = imm;
	ptrdiff_t d = *p;
	
	return (uint8_t*)imm + d + sizeof(void*);
}
void parambind_i_bind_func_d32(void *imm, void *func) 
{
	int32_t *p = imm;
	ptrdiff_t d = (uint8_t*)func - ((uint8_t*)imm + 4);
	*p = d;
}
void *parambind_i_unbind_func_d32(void *imm) 
{
	int32_t *p = imm;
	ptrdiff_t d = *p;
	void *func = (uint8_t*)imm + d + 4;

	return (uint8_t*)imm + d + 4;
}


void *parambind_i_bind_one(
	size_t template_size, void *template,
	ptrdiff_t func_idx, void *func,
	ptrdiff_t arg_idx, void *arg)
{
	void *code = parambind_i_alloc(template_size);

	if (code)
	{
		memcpy(code, template, template_size);

		void **p = (void*)((uint8_t*)code + arg_idx);
		*p = arg;

		parambind_i_bind_func_d((uint8_t*)code + func_idx, func);
	}

	return code;
}


void *parambind_i_unbind_one(
	void *code,
	ptrdiff_t func_idx,
	ptrdiff_t arg_idx, void **arg)
{
	if (code)
	{
		void **p = (void*)((uint8_t*)code + arg_idx);
		
		if (arg) *arg = *p;
		void *func = parambind_i_unbind_func_d((uint8_t*)code + func_idx);

		parambind_i_free(code);

		return func;
	}
	else
		return NULL;
}

void *parambind_i_bind_one_abs(
	size_t template_size, void *template,
	ptrdiff_t func_idx, void *func,
	ptrdiff_t arg_idx, void *arg)
{
	void *code = parambind_i_alloc(template_size);

	if (code)
	{
		memcpy(code, template, template_size);

		void **p = (void*)((uint8_t*)code + arg_idx);
		*p = arg;

		p = (void*)((uint8_t*)code + func_idx);
		*p = func;
	}

	return code;
}


void *parambind_i_unbind_one_abs(
	void *code,
	ptrdiff_t func_idx,
	ptrdiff_t arg_idx, void **arg)
{
	if (code)
	{
		void **p;
		
		if (arg)
		{
			p = (void*)((uint8_t*)code + arg_idx);
			*arg = *p;
		}

		p = (void*)((uint8_t*)code + func_idx);
		void *func = *p;

		parambind_i_free(code);

		return func;
	}
	else
		return NULL;
}
