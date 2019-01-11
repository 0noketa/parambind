/* 2012 8/9 .. 2015 5/16 */
/* only for 386+cdecl */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambind_i.h"


#ifdef FOR_I386_CDECL
#define ARG_IDX 5
#define FUNC_IDX 11
#else
#define ARG_IDX 4
#define FUNC_IDX 10
#endif


#ifdef FOR_I386_CDECL
void *bind_binary_p2
#else
void *bind_binary_p1
#endif
(void *f, void *arg){
	static uint8_t template[24] ={
		/* pop edx */
		0x5A,
		/* pop eax */
		0x58,
		/* push eax */
		0x50,
#ifdef FOR_I386_CDECL
		/* push edx */
		0x52,
#endif
		/* push imm */
		0x68, 0,0,0,0,
		/* push eax */
		0x50,
		/* call imm(rel) */
		0xE8, 0,0,0,0,
#ifdef FOR_I386_CDECL
		/* pop edx */
		0x5E,
		/* pop edx */
		0x5E,
#endif
		/* ret */
		0xC3
	};
	void *code =parambind_alloc(24);
	if(code){
		memcpy(code, template, 18+2);
		*(void**)((uint32_t)code+ARG_IDX) =arg;
		*(uint32_t*)((uint32_t)code+FUNC_IDX) =
			(uint32_t)f - (uint32_t)((uint32_t)code+FUNC_IDX+4);
	}
	
	return code;
}
#ifdef FOR_I386_CDECL
void *unbind_binary_p2
#else
void *unbind_binary_p1
#endif
(void *code, void *f, void *arg){
	if(code){
		*(uint32_t*)arg= *(uint32_t*)((uint32_t)code+ARG_IDX);
		*(uint32_t*)f= *(uint32_t*)((uint32_t)code+FUNC_IDX) + (uint32_t)code+FUNC_IDX+4;
		
		parambind_free(code);
	}
	
	return 0;
}
#undef ARG_IDX
#undef FUNC_IDX



#ifdef FOR_I386_CDECL
#define ARG_IDX 6
#define FUNC_IDX 11
#else
#define ARG_IDX 5
#define FUNC_IDX 10
#endif

#ifdef FOR_I386_CDECL
void *bind_binary_p1
#else
void *bind_binary_p2
#endif
(void *f, void *arg){
	static uint8_t template[24] ={
		/* pop edx */
		0x5A,
		/* pop eax */
		0x58,
		/* push eax */
		0x50,
#ifdef FOR_I386_CDECL
		/* push edx */
		0x52,
#endif
		/* push eax */
		0x50,
		/* push imm */
		0x68, 0,0,0,0,
		/* call imm(rel) */
		0xE8, 0,0,0,0,
#ifdef FOR_I386_CDECL
		/* pop edx */
		0x5E,
		/* pop edx */
		0x5E,
#endif
		/* ret */
		0xC3
	};
	void *code =parambind_alloc(24);
	if(code){
		memcpy(code, template, 18+2);
		*(void**)((uint32_t)code+ARG_IDX) =arg;
		*(uint32_t*)((uint32_t)code+FUNC_IDX) =(uint32_t)f - ((uint32_t)code+FUNC_IDX+4);
	}
	
	return code;
}
#ifdef FOR_I386_CDECL
void *unbind_binary_p1
#else
void *unbind_binary_p2
#endif
(void *code, void *f, void *arg){
	if(code){
		*(uint32_t*)arg= *(uint32_t*)((uint32_t)code+ARG_IDX);
		*(uint32_t*)f= *(uint32_t*)((uint32_t)code+FUNC_IDX) + ((uint32_t)code+FUNC_IDX+4);
		
		parambind_free(code);
	}
	
	return 0;
}
#undef ARG_IDX
#undef FUNC_IDX


#define ARG_IDX 1
#define FUNC_IDX 6
void *pack_anary(void *f, void *arg){
	static uint8_t template[16] ={
		/* push imm */
		0x68, 0,0,0,0,
		/* call imm(rel) */
		0xE8, 0,0,0,0,
#ifdef FOR_I386_CDECL
		/* pop edx */
		0x5E,
#endif
		/* ret */
		0xC3
	};
	void *code =parambind_alloc(16);
	if(code){
		memcpy(code, template, 12);
		*(void**)((uint32_t)code+ARG_IDX) =arg;
		*(uint32_t*)((uint32_t)code+FUNC_IDX) =(uint32_t)f - ((uint32_t)code+FUNC_IDX+4);
	}
	
	return code;
}
void *unpack_anary(void *code, void *f, void *arg){
	if(code){
		*(uint32_t*)arg= *(uint32_t*)((uint32_t)code+ARG_IDX);
		*(uint32_t*)f= *(uint32_t*)((uint32_t)code+FUNC_IDX) + ((uint32_t)code+FUNC_IDX+4);
		
		parambind_free(code);
	}
	
	return 0;
}
#undef ARG_IDX
#undef FUNC_IDX


void *free_binder(void* p)
	{ if(p)parambind_free(p); return 0; }
