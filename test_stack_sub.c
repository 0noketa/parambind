#include <stdlib.h>
#include "include/parambinder.h"
#include "test_stack_sub.h"

static Stack *Stack_del(Stack *stk);
static void *Stack_push(Stack *stk, void *i);
static void *Stack_pop(Stack *stk);


Stack *newStack(int len){
	Stack *stk= malloc(sizeof(Stack));

	if(stk){
		if(
			(stk->del= Stack_del ) &&
			(stk->push= bind_binary_p1(Stack_push, stk) ) &&
			(stk->pop= pack_anary(Stack_pop, stk) ) &&
			(stk->stack= malloc(sizeof(void*)*len) )
		){
			stk->sp= stk->stack;
			stk->spTail= stk->stack + len;
		}else
			stk= stk->del(stk);
	}

	return stk;
}

static Stack *Stack_del(Stack *stk){
	if(stk){
		if(stk->push) free_binder(stk->push);
		if(stk->pop) free_binder(stk->pop);
		if(stk->stack) free(stk->stack);

		free(stk);
	}
	
	return 0;
}

static void *Stack_push(Stack *stk, void *i){
	if(stk && stk->sp < stk->spTail )
		*(stk->sp)++= i;

	return i;
}

static void *Stack_pop(Stack *stk){
	void *i;
	if(stk && stk->stack < stk->sp )
		i= *--(stk->sp);
	else
		i= 0;

	return i;
}
