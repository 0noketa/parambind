#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "include/parambind.h"
#include "test_stack_sub.h"

int main()
{
	if (!parambind_init()) return 1;

	IntStack *stk1, *stk2;

	stk1 = newIntStack(4);
	stk2 = newIntStack(4);
	
	if (stk1 && stk2)
	{
		intptr_t (*push)(intptr_t), (*pop)(void);
		intptr_t a,b,c, d,e,f;

		push = stk1->push;
		push(1),push(3),push(5);

		push = stk2->push;
		push(2),push(4),push(6);

		pop = stk1->pop;
		e=pop(); c=pop(); a=pop();

		pop = stk2->pop;
		f=pop(); d=pop(); b=pop();

		printf("%d,%d,%d, %d,%d,%d\n", a,b,c, d,e,f);
	}

	if(stk1) stk1 = stk1->del(stk1);
	if(stk2) stk2 = stk2->del(stk2);

	parambind_clean();

	return 0;
}
