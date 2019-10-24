/* only for x64+vectorcall */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "parambind_i.h"


typedef enum {
	REG_RAX, REG_RCX, REG_RDX, REG_RBX,
	REG_RSI, REG_RDI, 
	REG_R8, REG_R9, REG_R10, REG_R11
} X64Register;

typedef enum {
	CCONVENTION_VECTORCALL,
	CCONVENTION_AMD64
} CConvention;

static const uint8_t
	template_alias[] = {
		/* jmp imm */
		0x69, 0,0,0,0
	},
	template_stackTopToR10[] = {
		/* mov r10, [rsp] */
		0x4C, 0x8B, 0x14, 0x24
	},
	template_r10ToStackTop[] = {
		/* mov [rsp], r10 */
		0x4C, 0x89, 0x14, 0x24
	},
	template_allocAuto[] = {
		/* sub rsp, imm8 */
		0x48, 0x83, 0xEC, 0
	},
	template_mov[] = {
		/* mov rax, rax */
		0x48, 0x89, 0xC0
	},
	template_toStack_rcx[] = {
		/* mov [rsp+imm], rcx */
		0x48, 0x89, 0x4C, 0x24, 0
	},
	template_toStack_rdx[] = {
		/* mov [rsp+imm], rdx */
		0x48, 0x89, 0x54, 0x24, 0
	},
	template_toStack_rsi[] = {
		/* mov [rsp+imm], rsi */
		0x48, 0x89, 0x74, 0x24, 0
	},
	template_toStack_rdi[] = {
		/* mov [rsp+imm], rdi */
		0x48, 0x89, 0x7C, 0x24, 0
	},
	template_toStack_r8[] = {
		/* mov [rsp+imm], r8 */
		0x4C, 0x89, 0x44, 0x24, 0
	},
	template_toStack_r9[] = {
		/* mov [rsp+imm], r9 */
		0x4C, 0x89, 0x4C, 0x24, 0
	},
	template_toStack_r10[] = {
		/* mov [rsp+imm], r10 */
		0x4C, 0x89, 0x54, 0x24, 0
	},
	template_toStack_r11[] = {
		/* mov [rsp+imm], r11 */
		0x4C, 0x89, 0x5C, 0x24, 0
	},
	template_callWithR11[] = {
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0,
		/* call r11 */
		0x41, 0xFF, 0xD3
	},
	template_jmpWithR11[] = {
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0,
		/* jmp r11 */
		0x41, 0xFF, 0xE3
	},
	template_moveWithR11[] = {
		/* mov r11, [rsp+imm8] */
		0x4C, 0x8B, 0x5C, 0x24, 0,
		/* mov [rsp+imm8], r11 */
		0x4C, 0x89, 0x5C, 0x24, 0
	},
	template_fromStack_rcx[] = {
		/* mov rcx, [rsp+imm8] */
		0x48, 0x8B, 0x4C, 0x24, 0
	},
	template_fromStack_rdx[] = {
		/* mov rdx, [rsp+imm8] */
		0x48, 0x8B, 0x54, 0x24, 0
	},
	template_fromStack_rsi[] = {
		/* mov rsi, [rsp+imm8] */
		0x48, 0x8B, 0x74, 0x24, 0
	},
	template_fromStack_rdi[] = {
		/* mov rdi, [rsp+imm8] */
		0x48, 0x8B, 0x7C, 0x24, 0
	},
	template_fromStack_r8[] = {
		/* mov r8, [rsp+imm8] */
		0x4C, 0x8B, 0x44, 0x24, 0
	},
	template_fromStack_r9[] = {
		/* mov r9, [rsp+imm8] */
		0x4C, 0x8B, 0x4C, 0x24, 0
	},
	template_fromStack_r10[] = {
		/* mov r10, [rsp+imm8] */
		0x4C, 0x8B, 0x54, 0x24, 0
	},
	template_fromStack_r11[] = {
		/* mov r11, [rsp+imm8] */
		0x4C, 0x8B, 0x5C, 0x24, 0
	},
	template_ld_rcx[] = {
		/* mov rcx, imm */
		0x48, 0xB9, 0,0,0,0, 0,0,0,0
	},
	template_ld_rdx[] = {
		/* mov rdx, imm */
		0x48, 0xBA, 0,0,0,0, 0,0,0,0
	},
	template_ld_rdi[] = {
		/* mov rdi, imm */
		0x48, 0xBF, 0,0,0,0, 0,0,0,0
	},
	template_ld_rsi[] = {
		/* mov rsi, imm */
		0x48, 0xBE, 0,0,0,0, 0,0,0,0
	},
	template_ld_r8[] = {
		/* mov r8, imm */
		0x49, 0xB8, 0,0,0,0, 0,0,0,0
	},
	template_ld_r9[] = {
		/* mov r9, imm */
		0x49, 0xB9, 0,0,0,0, 0,0,0,0
	},
	template_ld_r10[] = {
		/* mov r10, imm */
		0x49, 0xBA, 0,0,0,0, 0,0,0,0
	},
	template_ld_r11[] = {
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0
	},
	template_initClosedArg[] = {
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0,
		/* mov [rsp+imm], r11 */
		0x4C, 0x89, 0x5C, 0x24, 0
	},
	template_deallocAuto[] = {
		/* add rsp, imm8 */
		0x48, 0x83, 0xC4, 0
	},
	template_ret[] = {
		0xC3
	};

const uint8_t *templates_toStack[] = {
	[REG_RCX] = template_toStack_rcx,
	[REG_RDX] = template_toStack_rdx,
	[REG_RSI] = template_toStack_rsi,
	[REG_RDI] = template_toStack_rdi,
	[REG_R8] = template_toStack_r8,
	[REG_R9] = template_toStack_r9,
	[REG_R10] = template_toStack_r10,
	[REG_R11] = template_toStack_r11
};

const uint8_t *templates_ld[] = {
	[REG_RCX] = template_ld_rcx,
	[REG_RDX] = template_ld_rdx,
	[REG_RSI] = template_ld_rsi,
	[REG_RDI] = template_ld_rdi,
	[REG_R8] = template_ld_r8,
	[REG_R9] = template_ld_r9,
	[REG_R10] = template_ld_r10,
	[REG_R11] = template_ld_r11
};

const uint8_t *templates_fromStack[] = {
	[REG_RCX] = template_fromStack_rcx,
	[REG_RDX] = template_fromStack_rdx,
	[REG_RSI] = template_fromStack_rsi,
	[REG_RDI] = template_fromStack_rdi,
	[REG_R8] = template_fromStack_r8,
	[REG_R9] = template_fromStack_r9,
	[REG_R10] = template_fromStack_r10,
	[REG_R11] = template_fromStack_r11
};

const X64Register regs_vectorcall[] = {
	REG_RCX, REG_RDX, REG_R8, REG_R9
};
const X64Register regs_amd64[] = {
	REG_RSI, REG_RDI, REG_RDX, REG_RCX, REG_R8, REG_R9
};


uint8_t *ptrOrNull(uint8_t *p, ptrdiff_t d)
{
	return p ? p + d : NULL;
}

static int countRegsForArgs(CConvention c)
{
	return c == CCONVENTION_VECTORCALL ? 4
		: /* AMD64 */ 6;
}
static X64Register *regsForArgs(CConvention c)
{
	return c == CCONVENTION_VECTORCALL ? regs_vectorcall
		: /* AMD64 */ regs_amd64;
}
static int countRegsUsedForArgs(int argc, CConvention c)
{
	int regs = countRegsForArgs(c);

	return argc < regs ? argc : regs;
}
static size_t sizeofVargs(int argc, CConvention convention)
{
	int regs = countRegsForArgs(convention);
	uint8_t size = argc * 8;

	if (convention == CCONVENTION_VECTORCALL)
	{
		if (size % 16 == 0) size += 8;
		if (size <= regs * 8) size = regs * 8 + 8;		
	}
	else
	{
		size = argc >= regs
			? size - regs * 8
			: 0;
	}
	
	return size;
}
/* [rsp+?] */
static int8_t distanceofStackArg(int idx, CConvention convention)
{
	return idx * 8
		- (convention == CCONVENTION_VECTORCALL
			? 0
			: countRegsForArgs(convention) * 8);
}
static bool hasStackSpaceForRegArgs(CConvention c)
{
	return c == CCONVENTION_VECTORCALL;
}

static size_t write_any(void *dst, size_t size, uint8_t *template)
{
	if (dst) memcpy(dst, template, size);

	return size;
}

static size_t write_allocAuto(void *dst, uint8_t size)
{
	if (size == 0) return 0;

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
	if (size == 0) return 0;

	if (dst)
	{
		memcpy(dst, template_deallocAuto, sizeof(template_deallocAuto));

		uint8_t *p = (uint8_t*)dst + 3;
		*p = size;
	}

	return sizeof(template_deallocAuto);
}
static size_t write_toStack(void *dst, X64Register reg, int8_t d)
{
	if (dst)
	{
		memcpy(dst, templates_toStack[reg],
			sizeof(template_toStack_rcx));

		int8_t *p = (int8_t*)dst + 4;
		*p = d;
	}

	return sizeof(template_toStack_rcx);
}
static size_t write_fromStack(void *dst, X64Register reg, int8_t d)
{
	if (dst)
	{
		memcpy(dst, templates_fromStack[reg],
			sizeof(template_fromStack_rcx));

		int8_t *p = (int8_t*)dst + 4;
		*p = d;
	}

	return sizeof(template_fromStack_rcx);
}
static size_t write_regArgToStack(
	void *dst,
	int regIdx, int8_t d, 
	CConvention convention)
{
	return write_toStack(dst, regsForArgs(convention)[regIdx], d);
}
static size_t write_regArgFromStack(
	void *dst, 
	int regIdx, int8_t d, 
	CConvention convention)
{
	return write_fromStack(dst, regsForArgs(convention)[regIdx], d);
}

static size_t write_mov(void *dst, X64Register dstReg, X64Register srcReg)
{
	size_t r = write_any(dst, sizeof(template_mov), template_mov);

	if (dst)
	{
		uint8_t *p = dst;

		if (dstReg >= REG_R8)
		{
			p[0] |= 0x01;
			p[2] |= dstReg - REG_R8;
		}
		else if (dstReg >= REG_RSI)
		{
			p[2] |= 0x06 | (dstReg - REG_RSI);
		}
		else
		{
			p[2] |= dstReg;
		}

		if (srcReg >= REG_R8)
		{
			p[0] |= 0x04;
			p[2] |= (srcReg - REG_R8) << 3;
		}
		else if (srcReg >= REG_RSI)
		{
			p[2] |= 0x30 | (srcReg - REG_RSI) << 3;
		}
		else
		{
			p[2] |= srcReg << 3;
		}
		
	}

	return r;
}
static size_t write_moveWithR11(void *dst, int8_t _to, int8_t _from);
static size_t write_shiftArg(void *dst, int dstIdx, int srcIdx, CConvention convention)
{
	int nRegs = countRegsForArgs(convention);
	X64Register *regs = regsForArgs(convention);

	uint8_t *p = dst;
	ptrdiff_t d = 0;

	if (dstIdx < nRegs && srcIdx < nRegs)
	{
		d += write_mov(ptrOrNull(p, d), regs[dstIdx], regs[srcIdx]);
	}
	else if (dstIdx < nRegs)
	{
		d += write_fromStack(ptrOrNull(p, d),
			regs[dstIdx], 
			distanceofStackArg(srcIdx, convention));
	}
	else if (srcIdx < nRegs)
	{
		d += write_toStack(ptrOrNull(p, d),
			regs[srcIdx],
			distanceofStackArg(dstIdx, convention));
	}
	else
	{
		d += write_moveWithR11(ptrOrNull(p, d),
			distanceofStackArg(dstIdx, convention),
			distanceofStackArg(srcIdx, convention));
	}
	
	return d;
}

static size_t write_ld(void *dst, X64Register reg, void *arg)
{
	if (dst)
	{
		memcpy(dst, 
			templates_ld[reg], 
			sizeof(template_ld_rcx));

		void **p = (void*)((uint8_t*)dst + 2);
		*p = arg;
	}
	return sizeof(template_ld_rcx);

}

/* arg for inner call */
static size_t write_initClosedArg(void *dst, int idx, void *arg, CConvention convention)
{
	if (!dst)
	{
		return idx < countRegsForArgs(convention)
			? sizeof(template_ld_rcx)
			: sizeof(template_initClosedArg);
	}

	if (idx < countRegsForArgs(convention))
	{
		return write_ld(dst, regsForArgs(convention)[idx], arg);
	}
	else
	{
		memcpy(dst, template_initClosedArg, sizeof(template_initClosedArg));

		int8_t *d = (void*)((int8_t*)dst + 14);
		*d = convention == CCONVENTION_VECTORCALL
				? idx * 8
				: (idx - countRegsForArgs(convention)) * 8;

		void **p = (void*)((uint8_t*)dst + 2);
		*p = arg;

		return sizeof(template_initClosedArg);
	}	
}
static size_t write_initClosedArg_vectorcall(void *dst, int idx, void *arg)
{
	return write_initClosedArg(dst, idx, arg, CCONVENTION_VECTORCALL);
}
static size_t write_initClosedArg_amd64(void *dst, int idx, void *arg)
{
	return write_initClosedArg(dst, idx, arg, CCONVENTION_AMD64);
}

static size_t read_ld(void *src, X64Register reg, void **out_imm)
{
	void **p = (void*)((uint8_t*)src + 2);
	if (out_imm) *out_imm = *p;

	return sizeof(template_ld_rcx);
}

static size_t read_initClosedArg(void *src, int idx, void **out_arg, CConvention convention)
{
	if (idx < countRegsForArgs(convention))
	{
		return read_ld(src, regsForArgs(convention)[idx], out_arg);
	}
	else
	{
		void **p = (void*)((uint8_t*)src + 2);
		if (out_arg) *out_arg = *p;

		return sizeof(template_initClosedArg);
	}	
}
static size_t read_initClosedArg_vectorcall(void *dst, int idx, void **out_arg)
{
	return read_initClosedArg(dst, idx, out_arg, CCONVENTION_VECTORCALL);
}
static size_t read_initClosedArg_amd64(void *dst, int idx, void **out_arg)
{
	return read_initClosedArg(dst, idx, out_arg, CCONVENTION_AMD64);
}

static size_t write_moveWithR11(void *dst, int8_t _to, int8_t _from)
{
	if (dst)
	{
		memcpy(dst, template_moveWithR11, sizeof(template_moveWithR11));

		int8_t *p = dst;

		p[4] = _from;
		p[9] = _to;
	}

	return sizeof(template_moveWithR11);
}

/* memcpy */
static size_t write_copyWithR11(void *dst, int8_t _to, int8_t _from, int elements)
{
	if (elements <= 0) return 0;

	ptrdiff_t d = 0;
	uint8_t *p = dst;

	if (_to < _from)
	{
		for (int i = 0; i < elements; ++i)
		{
			d += write_moveWithR11(ptrOrNull(p, d),
				_to + i * 8,
				_from + i * 8);
		}
	}
	else
	{
		for (int i = elements; i-- > 0;)
		{
			d += write_moveWithR11(ptrOrNull(p, d),
				_to + i * 8,
				_from + i * 8);
		}
	}
	

	return d;
}
/* (0, 1, 2) -> (1, 2, 0) */
static size_t write_rol(void *dst, int elements)
{
	if (elements <= 1) return 0;

	ptrdiff_t d = 0;
	uint8_t *p = dst;

	d += write_fromStack(ptrOrNull(p, d), REG_R10, 0);

	d += write_copyWithR11(ptrOrNull(p, d), 0, 8, elements - 1);

	d += write_toStack(ptrOrNull(p, d), REG_R10, (elements - 1) * 8);

	return d;
}
static size_t write_ror(void *dst, int elements)
{
	if (elements <= 1) return 0;

	ptrdiff_t d = 0;
	uint8_t *p = dst;

	d += write_fromStack(ptrOrNull(p, d), REG_R10, (elements - 1) * 8);

	d += write_copyWithR11(ptrOrNull(p, d), 8, 0, elements - 1);

	d += write_toStack(ptrOrNull(p, d), REG_R10, 0);

	return d;
}

static size_t write_callWithR11(void *dst, void *f)
{
	if (dst)
	{
		memcpy(dst, template_callWithR11, sizeof(template_callWithR11));

		void **p = (void*)((uint8_t*)dst + 2);
		*p = f;
	}

	return sizeof(template_callWithR11);
}
static size_t write_jmpWithR11(void *dst, void *f)
{
	if (dst)
	{
		memcpy(dst, template_jmpWithR11, sizeof(template_jmpWithR11));

		void **p = (void*)((uint8_t*)dst + 2);
		*p = f;
	}

	return sizeof(template_jmpWithR11);
}
static size_t read_callWithR11(void *src, void **out_f)
{
	if (src && out_f) 
	{
		void **p = (void*)((uint8_t*)src + 2);
		*out_f = *p;
	}

	return sizeof(template_callWithR11);
}

static size_t write_ret(void *dst)
{
	return write_any(dst, sizeof(template_ret), template_ret);
}


static size_t write_callAndLeave(void *dst, void *f, size_t vargsSize)
{
	ptrdiff_t d = 0;
	uint8_t *p = dst;

	if (vargsSize == 0) 
	{
		d += write_jmpWithR11(ptrOrNull(p, d), f);
	}
	else
	{
		d += write_callWithR11(ptrOrNull(p, d), f);
		d += write_deallocAuto(ptrOrNull(p, d), vargsSize);
		d += write_ret(ptrOrNull(p, d));
	}

	return d;
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

	for(intptr_t i = 0; i < argc; ++i)
	{
		d += write_initClosedArg(ptrOrNull(p, d), i, argv[i], convention);
	}

	d += write_callAndLeave(ptrOrNull(p, d), f, vargsSize);

	return d;
}

static void *parambind_bind_a_fastcall(
	void *f, intptr_t argc, void *argv[],
	CConvention convention)
{
	/* 16 or more far args requires [rsp+imm32]
		(currently using [rsp+imm8]. imm is signed) */
	if (argc < 0 || argc > 15) return NULL;

	int regs = countRegsForArgs(convention);

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
	
	if (sizeofVargs(argc, convention) > 0)
		p += sizeof(template_allocAuto);

	for (int i = 0; i < argc; ++i)
	{
		p += read_initClosedArg(p, i, &argv[i], convention);
	}

	void *func;

	/* call and jmp has same layout */
	p += read_callWithR11(p, &func);

	parambind_i_free(code);

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


static size_t write_bind_rs_fastcall(
	void *dst,
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[],
	CConvention convention)
{
	intptr_t openedArgc = argc - closedArgc;
	int regs = countRegsForArgs(convention);
	int vargsSize = sizeofVargs(argc, convention);

	ptrdiff_t d = 0;
	uint8_t *p = dst;


	d += write_allocAuto(ptrOrNull(p, d), vargsSize);

	if (regs < openedArgc)
		d += write_copyWithR11(ptrOrNull(p, d),
				regs * 8,
				vargsSize + (1 + regs) * 8,
				openedArgc - regs);

	for (int i = 0; i < closedArgc; ++i)
	{
		d += write_initClosedArg(ptrOrNull(p, d),
				openedArgc + i, 
				closedArgv[i], 
				convention);
	}

	d += write_callAndLeave(ptrOrNull(p, d), f, vargsSize);

	return d;
}

static void *parambind_bind_rs_fastcall(
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[],
	CConvention convention)
{
	if (!f
		|| argc < 0 || argc > 7
		|| argc < closedArgc || !closedArgv)
		return NULL;

	size_t size = 
		write_bind_rs_fastcall(
			NULL,
			f, argc, closedArgc, closedArgv,
			convention);

	void *code = parambind_i_alloc(size);

	if (code)
	{
		write_bind_rs_fastcall(
			code,
			f, argc, closedArgc, closedArgv,
			convention);
	}

	return code;
}

void *parambind_bind_rs_vectorcall(
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[])
{
	return parambind_bind_rs_fastcall(
		f, argc, closedArgc, closedArgv,
		CCONVENTION_VECTORCALL);
}
void *parambind_bind_rs_amd64(
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[])
{
	return parambind_bind_rs_fastcall(
		f, argc, closedArgc, closedArgv,
		CCONVENTION_AMD64);
}

static size_t write_bind_ls_fastcall(
	void *dst,
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[],
	CConvention convention)
{
	intptr_t openedArgc = argc - closedArgc;
	int regs = countRegsForArgs(convention);
	size_t vargsSize = sizeofVargs(argc, convention);
	size_t openedVargsSize = sizeofVargs(openedArgc, convention);
	size_t closedVargsSize = closedArgc * 8
		- (hasStackSpaceForRegArgs(convention)
			? 0
			: regs < closedArgc
				? regs * 8
				: closedArgc * 8);

	ptrdiff_t d = 0;
	uint8_t *p = dst;

	if (openedVargsSize > 0)
	{
		if (argc <= regs)
		{ /* vectorcall-like (space for register args in stack) */
			d += write_moveWithR11(ptrOrNull(p, d),
					openedArgc * 8,
					0);
		}
		else
		{
			d += write_rol(ptrOrNull(p, d), openedArgc + 1);
		}		
	}

	d += write_allocAuto(ptrOrNull(p, d), closedVargsSize);

	for (int i = regs < openedArgc 
			? regs 
			: openedArgc; 
		i-- > 0;
		)
	{
		d += write_shiftArg(ptrOrNull(p, d),
				closedArgc + i,
				i,
				convention);
	}

	for (int i = 0; i < closedArgc; ++i)
	{
		d += write_initClosedArg(ptrOrNull(p, d),
				i, 
				closedArgv[i], 
				convention);
	}

	if (vargsSize > 0)
	{
		d += write_callWithR11(ptrOrNull(p, d), f);

		d += write_deallocAuto(ptrOrNull(p, d), closedVargsSize);

		if (openedVargsSize > 0)
		{
			/* 
			d += write_ror(ptrOrNull(p, d), openedArgc + 1);
			*/
			d += write_moveWithR11(ptrOrNull(p, d),
				0,
				openedArgc * 8);
		}

		d += write_ret(ptrOrNull(p, d));
	}
	else
	{
		d += write_jmpWithR11(ptrOrNull(p, d), f);
	}
	

	return d;
}

static void *parambind_bind_ls_fastcall(
	void *f, intptr_t argc,
	intptr_t closedArgc, void *closedArgv[],
	CConvention convention)
{
	if (!f
		|| argc < 0 || argc > 15
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


/* stub */
static size_t write_wrap(
	void *dst,
	void *f, intptr_t argc,
	CConvention dstConvention,
	CConvention srcConvention)
{
	int srcRegs = countRegsForArgs(srcConvention);
	int dstRegs = countRegsForArgs(dstConvention);

	ptrdiff_t d = 0;
	uint8_t *p = dst;

	/* store register args for this function */

	for (int i = 0; i < argc && i < dstRegs; ++i)
	{
		d += write_regArgToStack(ptrOrNull(p, d), 
				i, i * 8,
				dstConvention);
	}

	/* [caller, ...args] -> [...args, caller, ...regs] */

	d += write_fromStack(ptrOrNull(p, d), REG_R10, 0);
	d += write_allocAuto(ptrOrNull(p, d), srcRegs * 8);
	d += write_copyWithR11(ptrOrNull(p, d), 0, (srcRegs + 1) * 8, argc);
	d += write_toStack(ptrOrNull(p, d), REG_R10, argc * 8);

	for (int i = 0; i < srcRegs; ++i)
	{
		d += write_regArgToStack(ptrOrNull(p, d), 
				i, 
				(argc + 1 + i) * 8,
				srcConvention);
	}

	/* load register args for f */

	for (int i = 0; i < argc && i < srcRegs; ++i)
	{
		d += write_regArgFromStack(ptrOrNull(p, d), 
				i, i * 8,
				srcConvention);
	}

	/* if amd64->vectorcall, remove register args from stack at here */

	/* call */

	d += write_callWithR11(ptrOrNull(p, d), f);

	/* [...args, caller, ...regs] -> [caller, ...args] */

	d += write_fromStack(ptrOrNull(p, d), REG_R10, argc * 8);

	for (int i = 0; i < srcRegs; ++i)
	{
		d += write_regArgFromStack(ptrOrNull(p, d), 
				i, 
				(argc + 1 + i) * 8,
				srcConvention);
	}

	d += write_copyWithR11(ptrOrNull(p, d), (srcRegs + 1) * 8, 0, argc);

	/* if amd64->vectorcall, this block was not just srcRegs * 8 */
	d += write_deallocAuto(ptrOrNull(p, d), srcRegs * 8);

	d += write_toStack(ptrOrNull(p, d), REG_R10, 0);

	/* ret */

	d += write_ret(ptrOrNull(p, d));

	return d;
}

static void *parambind_wrap(
	void *f, intptr_t argc,
	CConvention dstConvention,
	CConvention srcConvention)
{
	if (!f || argc < 0 || argc > 7)
		return NULL;

	size_t size = 
		write_wrap(
			NULL,
			f, argc,
			dstConvention,
			srcConvention);

	void *code = parambind_i_alloc(size);

	if (code)
	{
		write_wrap(
			code,
			f, argc,
			dstConvention,
			srcConvention);
	}

	return code;
}
void *parambind_wrapas0_vectorcall_amd64(void *f, intptr_t argc)
{
	return parambind_wrap(
			f, argc,
			CCONVENTION_VECTORCALL,
			CCONVENTION_AMD64);
}
void *parambind_wrapas0_amd64_vectorcall(void *f, intptr_t argc)
{
	return parambind_wrap(
			f, argc,
			CCONVENTION_AMD64,
			CCONVENTION_VECTORCALL);
}
void *parambind_wrapas_amd64_vectorcall(void *f, intptr_t argc)
{
	const uint8_t template[] = {
		/* mov r9, rcx */
		0x49, 0x89, 0xC9,
		/* mov r8, rdx */
		0x49, 0x89, 0xD0,
		/* mov rdx, rdi */
		0x48, 0x89, 0xFA,
		/* mov rcx, rsi */
		0x48, 0x89, 0xF1,
		/* sub rsp, 40 */
		0x48, 0x83, 0xEC, 40,
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0,
		/* call r11 */
		0x41, 0xFF, 0xD3,
		/* add rsp, 40 */
		0x48, 0x83, 0xC4, 40,
		/* ret */
		0xC3
	};

	if (argc > 4 || f == NULL) return NULL;

	uint8_t *code = parambind_i_alloc(sizeof(template));

	if (code)
	{
		void **p = (void*)(code + 18);

		*p = f;
	}

	return code;
}
void *parambind_wrapas_vectorcall_amd64(void *f, intptr_t argc)
{
	const uint8_t template[] = {
		/* sub rsp, 40 */
		0x48, 0x83, 0xEC, 40,
		/* mov [rsp], rsi */
		0x48, 0x89, 0x34, 0x24,
		/* mov [rsp+8], rdi */
		0x48, 0x89, 0x7C, 0x24, 8,
		/* mov rsi, rcx */
		0x48, 0x89, 0xCE,
		/* mov rdi, rdx */
		0x48, 0x89, 0xD7,
		/* mov rdx, r8 */
		0x4C, 0x89, 0xC2,
		/* mov rcx, r9 */
		0x4C, 0x89, 0xC9,
		/* mov r11, imm */
		0x49, 0xBB, 0,0,0,0, 0,0,0,0,
		/* call r11 */
		0x41, 0xFF, 0xD3,
		/* mov rsi, [rsp] */
		0x48, 0x8B, 0x34, 0x24,
		/* mov rdi,[rsp+8] */
		0x48, 0x8B, 0x7C, 0x24, 8,
		/* add rsp, 40 */
		0x48, 0x83, 0xC4, 40,
		/* ret */
		0xC3
	};

	if (argc > 4 || f == NULL) return NULL;

	uint8_t *code = parambind_i_alloc(sizeof(template));

	if (code)
	{
		void **p = (void*)(code + 27);

		*p = f;
	}

	return code;
}
