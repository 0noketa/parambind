#include <stdlib.h>

#define parambind_opt_aliases
#include "include/parambind.h"
#include "test_stack_sub.h"

static Stack *Stack_del(Stack *stk);
static intptr_t Stack_push(Stack *stk, void *i);
static void *Stack_pop(Stack *stk);
static intptr_t Stack_len(Stack *stk);


Stack *newStack(uintptr_t len)
{
	Stack *stk = calloc(1, sizeof(Stack));

	if (stk)
	{
		if ((stk->del = Stack_del)
			&& (stk->push = bind_l(Stack_push, stk))
			&& (stk->pop = bind_u(Stack_pop, stk))
			&& (stk->len = bind_u(Stack_len, stk))
			&& (stk->stack = malloc(sizeof(void*)*len)))
		{
			stk->sp = stk->stack;
			stk->spTail = stk->stack + len;
		}else
			stk = stk->del(stk);
	}

	return stk;
}

static Stack *Stack_del(Stack *stk)
{
	if (stk)
	{
		if(stk->push) unbind_l(stk->push, NULL);
		if(stk->pop) unbind_u(stk->pop, NULL);
		if(stk->len) unbind_u(stk->len, NULL);
		if(stk->stack) free(stk->stack);

		free(stk);
	}
	
	return 0;
}

static intptr_t Stack_push(Stack *stk, void *i)
{
	intptr_t r = stk && stk->sp < stk->spTail;

	if (r)
		*stk->sp++ = i;

	return r;
}

static void *Stack_pop(Stack *stk)
{
	void *i;

	if (stk && stk->stack < stk->sp)
		i = *--stk->sp;
	else
		i= 0;

	return i;
}

static intptr_t Stack_len(Stack *stk)
{
	return (intptr_t)(stk->sp - stk->stack);
}
