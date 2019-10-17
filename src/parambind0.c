/* 2019/10/17 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambind_i.h"


int parambind_init()
{
	return sizeof(int) == 4
		&& sizeof(void*) == 4
		&& sizeof(void (*)()) == 4
		&& sizeof(void (* (*)() )()) == 4
		&& parambind_i_init();
}
int parambind_clean()
{
	return parambind_i_clean();
}


void parambind_i_bind_func(void *imm, void *func) 
{
	void **p = imm;
	*p = (uint8_t*)func - ((uintptr_t)imm + 4);
}
void *parambind_i_unbind_func(void *imm) 
{
	void **p = (uint8_t*)imm;
	
	return (uint8_t*)*p + ((uintptr_t)imm + 4);
}


void *parambind_i_bind_one(
	size_t template_size, void *template,
	uint32_t func_idx, void *func,
	uint32_t arg_idx, void *arg)
{
	void *code = parambind_i_alloc(template_size);

	if (code)
	{
		memcpy(code, template, template_size);

		void **p = (uint8_t*)code + arg_idx;
		*p = arg;

		parambind_i_bind_func((uint8_t*)code + func_idx, func);
	}

	return code;
}


void *parambind_i_unbind_one(
	void *code,
	uint32_t func_idx,
	uint32_t arg_idx, void **arg)
{
	if (code)
	{
		void **p = (uint8_t*)code + arg_idx;
		
		if (arg) *arg = *p;
		void *func = parambind_i_unbind_func((uint8_t*)code + func_idx);

		parambind_i_free(code);

		return func;
	}
	else
		return NULL;
}
