#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define parambind_opt_aliases
#include "include/parambind.h"


intptr_t func6(
    intptr_t a, intptr_t b, intptr_t c, intptr_t d,
    intptr_t e, intptr_t f)
{
    printf("func6(%04x, %04x,  %04x, %04x,  %04x, %04x)\n",
        (int)a,  (int)b,  (int)c,  (int)d,
        (int)e,  (int)f);

    return a + b + c + d + e + f;
}


int main()
{
    if (!parambind_init()) return 1;


    intptr_t (*p0)(void);
    intptr_t (*p2)(intptr_t, intptr_t);
    intptr_t (*p4)(intptr_t, intptr_t, intptr_t, intptr_t);
    intptr_t (*p6)(
        intptr_t, intptr_t, intptr_t, intptr_t,
        intptr_t, intptr_t);
 
    intptr_t args[] = { 1, 2, 3, 4, 5, 6 };
    void *pArgs = args;

    p0 = bind_ls(func6, 6, 6, pArgs);
    p2 = bind_ls(func6, 6, 4, pArgs);
    p4 = bind_ls(func6, 6, 2, pArgs);
    p6 = bind_ls(func6, 6, 0, pArgs);

    p0();
    p2(0x100, 0x200);
    p4(0x300, 0x400, 0x500, 0x600);
    p6(0x700, 0x800, 0x900, 0xA00, 0xB00, 0xC00);

    p0 = bind_rs(func6, 6, 6, pArgs);
    p2 = bind_rs(func6, 6, 4, pArgs);
    p4 = bind_rs(func6, 6, 2, pArgs);
    p6 = bind_rs(func6, 6, 0, pArgs);

    p0();
    p2(0x100, 0x200);
    p4(0x300, 0x400, 0x500, 0x600);
    p6(0x700, 0x800, 0x900, 0xA00, 0xB00, 0xC00);


    parambind_clean();

    return 0;
}
