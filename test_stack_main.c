#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "test_stack_sub.h"

int main(){
	IntStack *stk1, *stk2;

	stk1= newStack(4);
	stk2= newStack(4);
	
	if( stk1 && stk2 ){
		int (*push)(int), (*pop)(void);
		int a,b,c, d,e,f;

		push= stk1->push;
		push(1),push(3),push(5);

		push= stk2->push;
		push(2),push(4),push(6);

		pop= stk1->pop;
		e=pop(); c=pop(); a=pop();

		pop= stk2->pop;
		f=pop(); d=pop(); b=pop();

		printf("%d,%d,%d, %d,%d,%d\n", a,b,c, d,e,f);
	}

	if(stk1) stk1= stk1->del(stk1);
	if(stk2) stk2= stk2->del(stk2);

	return 0;
}
