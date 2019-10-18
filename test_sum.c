#include <stdio.h>
#include <stdlib.h>

#define parambind_opt_aliases
#include "include/parambind.h"

#include "test_stack_sub.h"


intptr_t pass(intptr_t v)
{
    printf("%d \n", (int)v);
    return v;
}

intptr_t sum(intptr_t v, intptr_t (*f)())
{
    printf("%d +\n", (int)v);

    intptr_t v2 = f();

    return v + v2;
}


int main(int argc, char *argv[])
{
    if (argc < 2 || !parambind_init()) return 1;

    Stack *funcs = newStack(argc);


    void *p = bind_u(pass, (intptr_t)atoi(argv[1]));
    funcs->push(p);

    for (int i = 2; i < argc; ++i)
    {
        void *a[] = { (intptr_t)atoi(argv[i]), p };

        p = bind_a(sum, 2, a);
        funcs->push(p);
    }


    intptr_t (*f)() = p;
    int r = f();

    printf("= %d\n", r);


    while (funcs->len())
        parambind_free(funcs->pop());

    funcs->del(funcs);

    parambind_clean();

    return 0;
}
