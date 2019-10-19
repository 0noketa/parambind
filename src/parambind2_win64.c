/* 2019 10/20 */
/* only for x64+vectorcall */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambind_i.h"


typedef enum {
	CCONVENTION_VECTORCALL,
	CCONVENTION_AMD64
} CConvention;

static const uint8_t
	template_alias[] = {
		/* jmp imm */
		0x69, 0,0,0,0
	},
	template_allocAuto[] = {
		/* sub rsp, imm8 */
		0x48, 0x83, 0xEC, 0
	},
	template_initVarg_rcx[] = {
		/* mov [rsp+imm], rcx */
		0x48, 0x89, 0x4C, 0x24, 0
	},
	template_initVarg_rdx[] = {
		/* mov [rsp+imm], rdx */
		0x48, 0x89, 0x54, 0x24, 0
	},
	template_initVarg_rsi[] = {
		/* mov [rsp+imm], rsi */
		0x48, 0x89, 0x74, 0x24, 0
	},
	template_initVarg_rdi[] = {
		/* mov [rsp+imm], rdi */
		0x48, 0x89, 0x7C, 0x24, 0
	},
	template_initVarg_r8[] = {
		/* mov [rsp+imm], r8 */
		0x4C, 0x89, 0x44, 0x24, 0
	},
	template_initVarg_r9[] = {
		/* mov [rsp+imm], r9 */
		0x4C, 0x89, 0x4C, 0x24, 0
	},
	template_initVarg_r10[] = {
		/* mov [rsp+imm], r10 */
		0x4C, 0x89, 0x54, 0x24, 0
	},
	template_initVarg_r11[] = {
		/* mov [rsp+imm], r11 */
		0x4C, 0x89, 0x5C, 0x24, 0
	},
	template_funcLoader[] = {
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0,
		/* mov [rsp], r11 */
		0x4C, 0x89, 0x1C, 0x24
	},
	template_shiftArg[] = {
		/* mov r11, [rsp+imm8] */
		0x4C, 0x8B, 0x5C, 0x24, 0,
		/* mov [rsp+imm8], r11 */
		0x4C, 0x89, 0x5C, 0x24, 0
	},
	template_peekArgFromStack_rcx[] = {
		/* mov rcx, [rsp+imm8] */
		0x48, 0x8B, 0x4C, 0x24, 0
	},
	template_peekArgFromStack_rdx[] = {
		/* mov rdx, [rsp+imm8] */
		0x48, 0x8B, 0x54, 0x24, 0
	},
	template_peekArgFromStack_rsi[] = {
		/* mov rsi, [rsp+imm8] */
		0x48, 0x8B, 0x74, 0x24, 0
	},
	template_peekArgFromStack_rdi[] = {
		/* mov rdi, [rsp+imm8] */
		0x48, 0x8B, 0x7C, 0x24, 0
	},
	template_peekArgFromStack_r8[] = {
		/* mov r8, [rsp+imm8] */
		0x4C, 0x8B, 0x44, 0x24, 0
	},
	template_peekArgFromStack_r9[] = {
		/* mov r9, [rsp+imm8] */
		0x4C, 0x8B, 0x4C, 0x24, 0
	},
	template_movRcx[] = {
		/* mov rcx, imm */
		0x48, 0xB9, 0,0,0,0, 0,0,0,0
	},
	template_movRdx[] = {
		/* mov rdx, imm */
		0x48, 0xBA, 0,0,0,0, 0,0,0,0
	},
	template_movRdi[] = {
		/* mov rdi, imm */
		0x48, 0xBF, 0,0,0,0, 0,0,0,0
	},
	template_movRsi[] = {
		/* mov rsi, imm */
		0x48, 0xBE, 0,0,0,0, 0,0,0,0
	},
	template_movR8[] = {
		/* mov r8, imm */
		0x49, 0xB8, 0,0,0,0, 0,0,0,0
	},
	template_movR9[] = {
		/* mov r9, imm */
		0x49, 0xB9, 0,0,0,0, 0,0,0,0
	},
	template_movR10[] = {
		/* mov r10, imm */
		0x49, 0xBA, 0,0,0,0, 0,0,0,0
	},
	template_movR11[] = {
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0
	},
	template_closedArg[] = {
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0,
		/* mov [rsp+imm], r11 */
		0x4C, 0x89, 0x5C, 0x24, 0
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

const uint8_t *templates_initVarg_vectorcall[] = {
	template_initVarg_rcx,
	template_initVarg_rdx,
	template_initVarg_r8,
	template_initVarg_r9
};
const uint8_t *templates_initVarg_amd64[] = {
	template_initVarg_rsi,
	template_initVarg_rdi,
	template_initVarg_rdx,
	template_initVarg_rcx,
	template_initVarg_r8,
	template_initVarg_r9
};

const uint8_t *templates_movR_vectorcall[] = {
	template_movRcx,
	template_movRdx,
	template_movR8,
	template_movR9
};
const uint8_t *templates_movR_amd64[] = {
	template_movRsi,
	template_movRdi,
	template_movRdx,
	template_movRcx,
	template_movR8,
	template_movR9
};

const uint8_t *templates_peekArgFromStack_vectorcall[] = {
	template_peekArgFromStack_rcx,
	template_peekArgFromStack_rdx,
	template_peekArgFromStack_r8,
	template_peekArgFromStack_r9
};
const uint8_t *templates_peekArgFromStack_amd64[] = {
	template_peekArgFromStack_rsi,
	template_peekArgFromStack_rdi,
	template_peekArgFromStack_rdx,
	template_peekArgFromStack_rcx,
	template_peekArgFromStack_r8,
	template_peekArgFromStack_r9
};

uint8_t *ptrOrNull(uint8_t *p, ptrdiff_t d)
{
	return p ? p + d : NULL;
}

static size_t sizeofVargs(int argc, CConvention convention)
{
	int regs = registersForArgs(convention);
	uint8_t size = argc * 8;

	if (size % 16 == 0) size += 8;
	if (size <= regs * 8) size = regs * 8 + 8;

	return size;
}

static int registersForArgs(CConvention c)
{
	return c == CCONVENTION_VECTORCALL ? 4
		: /* AMD64 */ 6;
}
static uint8_t **templates_initVarg_for(CConvention c)
{
	return c == CCONVENTION_VECTORCALL ? templates_initVarg_vectorcall
		: /* AMD64 */ templates_initVarg_amd64;
}
static uint8_t **templates_movR_for(CConvention c)
{
	return c == CCONVENTION_VECTORCALL ? templates_movR_vectorcall
		: /* AMD64 */ templates_movR_amd64;
}
static uint8_t **templates_peekArgFromStack_for(CConvention c)
{
	return c == CCONVENTION_VECTORCALL ? templates_peekArgFromStack_vectorcall
		: /* AMD64 */ templates_peekArgFromStack_amd64;
}

static size_t write_allocAuto(void *dst, uint8_t size)
{
	if (dst)
	{
		memcpy(dst, template_allocAuto, sizeof(template_allocAuto));

		uint8_t *p = (uint8_t*)dst + 3;
		*p = size;
	}

	return sizeof(template_allocAuto);
}
static size_t write_deallocAuto(void *dst, uint8_t size)
{
	if (dst)
	{
		memcpy(dst, template_deallocAuto, sizeof(template_deallocAuto));

		uint8_t *p = (uint8_t*)dst + 3;
		*p = size;
	}

	return sizeof(template_deallocAuto);
}
static size_t write_initVarg(void *dst, uint8_t idx, CConvention convention)
{
	if (dst)
	{
		memcpy(dst, templates_initVarg_for(convention)[idx],
			sizeof(template_initVarg_rcx));

		uint8_t *p = (uint8_t*)dst + 4;
		*p = idx * 8 + 8;
	}

	return sizeof(template_initVarg_rcx);
}
static size_t write_peekArgFromStack(void *dst, uint8_t idx, CConvention convention)
{
	if (dst)
	{
		memcpy(dst, templates_peekArgFromStack_for(convention)[idx],
			sizeof(template_peekArgFromStack_rcx));

		uint8_t *p = (uint8_t*)dst + 4;

		*p = idx * 8;
	}

	return sizeof(template_peekArgFromStack_rcx);
}

static size_t write_funcLoader(void *dst, void *f)
{
	if (dst)
	{
		memcpy(dst, template_funcLoader, sizeof(template_funcLoader));

		void **p = (void*)((uint8_t*)dst + 2);
		*p = f;
	}

	return sizeof(template_funcLoader);
}

static size_t write_closedArg(void *dst, int idx, void *arg, CConvention convention)
{
	if (!dst)
	{
		return idx < registersForArgs(convention)
			? sizeof(template_movRcx)
			: sizeof(template_closedArg);
	}

	if (idx < registersForArgs(convention))
	{
		memcpy(dst, 
			templates_movR_for(convention)[idx], 
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
static size_t write_closedArg_vectorcall(void *dst, int idx, void *arg)
{
	return write_closedArg(dst, idx, arg, CCONVENTION_VECTORCALL);
}
static size_t write_closedArg_amd64(void *dst, int idx, void *arg)
{
	return write_closedArg(dst, idx, arg, CCONVENTION_AMD64);
}

static size_t read_closedArg(void *dst, int idx, void **arg, CConvention convention)
{
	if (idx < registersForArgs(convention))
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
static size_t read_closedArg_vectorcall(void *dst, int idx, void **arg)
{
	return read_closedArg(dst, idx, arg, CCONVENTION_VECTORCALL);
}
static size_t read_closedArg_amd64(void *dst, int idx, void **arg)
{
	return read_closedArg(dst, idx, arg, CCONVENTION_AMD64);
}

static size_t write_shiftArg(void *dst, uint8_t _from, uint8_t _to)
{
	if (dst)
	{
		memcpy(dst, template_shiftArg, sizeof(template_shiftArg));

		uint8_t *p = dst;

		p[4] = _from;
		p[9] = _to;
	}

	return sizeof(template_shiftArg);
}

static size_t write_peekAndCall(void *dst)
{
	if (dst)
	{
		memcpy(dst, template_peekAndCall, sizeof(template_peekAndCall));
	}

	return sizeof(template_peekAndCall);
}
static size_t write_ret(void *dst)
{
	if (dst) memcpy(dst, template_ret, sizeof(template_ret));

	return sizeof(template_ret);
}


static size_t write_bind_a_fastcall(
	void *dst,
	void *f, int argc, void *argv[],
	CConvention convention)
{
	uint8_t vargsSize = sizeofVargs(argc, convention);
	size_t d = 0;
	uint8_t *p = dst;

	d += write_allocAuto(ptrOrNull(p, d), vargsSize);

	d += write_funcLoader(ptrOrNull(p, d), f);

	for(intptr_t i = 0; i < argc; ++i)
	{
		d += write_closedArg(ptrOrNull(p, d), i, argv[i], convention);
	}

	d += write_peekAndCall(ptrOrNull(p, d));
	d += write_deallocAuto(ptrOrNull(p, d), vargsSize);
	d += write_ret(ptrOrNull(p, d));

	return d;
}

static void *parambind_bind_a_fastcall(
	void *f, intptr_t argc, void *argv[],
	CConvention convention)
{
	/* 16 or more far args requires [rsp+imm32]
		(currently using [rsp+imm8]. imm is signed) */
	if (argc < 0 || argc > 15) return NULL;

	int regs = registersForArgs(convention);

	size_t size =
		write_bind_a_fastcall(
			NULL,
			f, argc, argv,
			convention);

	uint8_t *code = parambind_i_alloc(size);
	
	if (code)
	{
		write_bind_a_fastcall(
			code,
			f, argc, argv,
			convention);
	}	

	return code;
}
void *parambind_bind_a_vectorcall(void *f, intptr_t argc, void *argv[])
{
	return parambind_bind_a_fastcall(f, argc, argv, CCONVENTION_VECTORCALL);
}
void *parambind_bind_a_amd64(void *f, intptr_t argc, void *argv[])
{
	return parambind_bind_a_fastcall(f, argc, argv, CCONVENTION_AMD64);
}

static void *parambind_unbind_a_fastcall(
	void *code, intptr_t argc, void *argv[],
	CConvention convention)
{
	if (code == NULL
		|| argc < 0 || argc > 15
		|| argc > 0 && argv == NULL)
		return NULL;

	uint8_t *p = code;
	
	p += sizeof(template_allocAuto);

	void *func = *(void**)(void*)(p + 2);

	p += sizeof(template_funcLoader);

	for (int i = 0; i < argc; ++i)
	{
		p += read_closedArg(p, i, &argv[i], convention);
	}

	return func;
}
void *parambind_unbind_a_vectorcall(void *code, intptr_t argc, void *argv[])
{
	return parambind_unbind_a_fastcall(code, argc, argv, CCONVENTION_VECTORCALL);
}
void *parambind_unbind_a_amd64(void *code, intptr_t argc, void *argv[])
{
	return parambind_unbind_a_fastcall(code, argc, argv, CCONVENTION_AMD64);
}


static size_t write_bind_ls_fastcall(
	void *dst,
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[],
	CConvention convention)
{
	intptr_t openedArgc = argc - closedArgc;
	int regs = registersForArgs(convention);
	int vargsSize = sizeofVargs(argc, convention);

	ptrdiff_t d = 0;
	uint8_t *p = dst;

	/* uses Varg_list for simplifying */
	for (int i = 0; i < openedArgc && i < regs; ++i)
	{
		d += write_initVarg(ptrOrNull(p, d), i, convention);
	}

	d += write_allocAuto(ptrOrNull(p, d), vargsSize);

	for (int i = 0; i < openedArgc; ++i)
	{
		d += write_shiftArg(ptrOrNull(p, d),
			vargsSize + (1 + i) * 8,
			(closedArgc + i) * 8);
	}

	/* init registers with shifted args */
	for (int i = closedArgc; i < argc && i < regs; ++i)
	{
		d += write_peekArgFromStack(ptrOrNull(p, d), i, convention);
	}

	for (int i = 0; i < closedArgc; ++i)
	{
		d += write_closedArg(ptrOrNull(p, d), i, closedArgv[i], convention);
	}

	d += write_funcLoader(ptrOrNull(p, d), f);
	d += write_peekAndCall(ptrOrNull(p, d));
	d += write_deallocAuto(ptrOrNull(p, d), vargsSize);
	d += write_ret(ptrOrNull(p, d));

	return d;
}

static void *parambind_bind_ls_fastcall(
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[],
	CConvention convention)
{
	if (!f
		|| argc < 0 || argc > 7
		|| argc < closedArgc || !closedArgv)
		return NULL;

	size_t size = 
		write_bind_ls_fastcall(
			NULL,
			f, argc, closedArgc, closedArgv,
			convention);

	void *code = parambind_i_alloc(size);

	if (code)
	{
		write_bind_ls_fastcall(
			code,
			f, argc, closedArgc, closedArgv,
			convention);
	}

	return code;
}

void *parambind_bind_ls_vectorcall(
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[])
{
	return parambind_bind_ls_fastcall(
		f, argc, closedArgc, closedArgv,
		CCONVENTION_VECTORCALL);
}
void *parambind_bind_ls_amd64(
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[])
{
	return parambind_bind_ls_fastcall(
		f, argc, closedArgc, closedArgv,
		CCONVENTION_AMD64);
}

