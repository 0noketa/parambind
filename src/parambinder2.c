/* .. 2015 5/16 */
/* only for 386+cdecl */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "parambinder_i.h"



#ifndef FOR_I386_CDECL
/* only dummies */
void *bind_left(void *f, int closedArgc, void *closedArgv[], int my_freeArgc);
void *pack(void *f, int argc, void* argv[]) {return 0;}
void *unpack(void *code, int argc, void **f, void* argv[]) {return 0;}
int rebind_on_packed(void *code, int argIdx, void *arg) {return 0;}
#else


static const int closedArg_len= 5, my_freeArg_len= 5, body_len= 12;
static const uint8_t
	template_closedArg[8]= {
		/* push imm */
		0x68, 0,0,0,0
	},
	template_my_freeArg[8]= {
		/* mov eax, esp */
		0x89, 0xE0,
		/* push [eax+imm8] */
		0xFF, 0x70, 0
	},
	template_body[16]= {
		/* call imm(rel) */
		0xE8, 0,0,0,0,
		/* add esp, imm32 */
		0x81, 0xC4, 0,0,0,0,
		/* ret */
		0xC3
	};

/*
 int f(int a, int b, int c, int d, int e);
 int binder[2]= {1,2};
 int (*f2)(int,int)= bind_left(f, 2,binder, 3);
 f2(3,4,5);  //equals to f(1,2,3,4,5)
*/

void *bind_left(void *f, int closedArgc, void *closedArgv[], int my_freeArgc){
	int argc= closedArgc + my_freeArgc;
	int size= closedArg_len*closedArgc + my_freeArg_len*my_freeArgc + body_len;
	uint8_t *code= 0;

	if( 0<=argc && argc<=32 && (code= parambinder_alloc(size)) ){
		uint32_t i;
		uint8_t *p= code;

		for(i=0; i<my_freeArgc; ++i, p+=my_freeArg_len){
			memcpy(p,template_my_freeArg, my_freeArg_len);
			*(uint8_t*)(p+4)= (my_freeArgc-i+i)*4;
		}
		for(i=0; i<closedArgc; ++i, p+=closedArg_len){
			memcpy(p,template_closedArg, closedArg_len);
			*(void**)(p+1)= closedArgv[closedArgc-i-1];
		}

		memcpy(p,template_body, body_len);
		*(uint32_t*)(p+1)= (uint32_t)f - (uint32_t)(p+1+4);
		*(uint32_t*)(p+7)= argc*4;
	}	

	return code;
}
void *pack(void *f, int argc, void* argv[]){
	uint32_t size= closedArg_len*argc + body_len;
	uint8_t *code= 0;

	if( 0<=argc && argc<=256 && (code= parambinder_alloc(size)) ){
		uint32_t i;
		uint8_t *p= code;
		for(i=0; i<argc; ++i, p+=closedArg_len){
			memcpy(p,template_closedArg, closedArg_len);
			*(void**)(p+1)= argv[argc-i-1];
		}

		memcpy(p,template_body, body_len);
		*(uint32_t*)(p+1)= (uint32_t)f - (uint32_t)(p+1+4);
		*(uint32_t*)(p+7)= argc*4;
	}	

	return code;
}
void *unpack(void *code, int argc, void **f, void* argv[]){
	int size= closedArg_len*argc + body_len;

	if( code ){
		int i;
		uint8_t *p= code;
		for(i=0; i<argc; ++i, p+=closedArg_len)
			argv[argc-i-1]= *(void**)(p+1);

		*(uint32_t*)f= (uint32_t)(p+1+4) + *(int*)(p+1);

		parambinder_free(code);
	}	

	return 0;
}

int rebind_packed(void *code, int argIdx, void *arg){
	if(code){
		uint8_t *p= (uint8_t*)code + closedArg_len*argIdx;
		
		*(void**)(p+1)= arg;
	}
	
	return !!code;
}


#endif
